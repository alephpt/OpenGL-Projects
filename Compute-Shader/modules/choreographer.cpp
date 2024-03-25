#include "./choreographer.h"
#include "choreographer.h"

World::World() { initGLFW(); }
World::~World() { 
    glDeleteProgram(shader_program);
    glDeleteTextures(1, &apparition);

    delete manifestation;
    delete witness;

    glfwTerminate();
}


void World::initGL() {
    glClearColor(0.1f, 0.3f, 0.23f, 1.0f);

    glViewport(0, 0, WIDTH, HEIGHT);

    glDisable(GL_DEPTH_TEST);

    shader_program = initShaders();

    Reflection reflection;  // This is essentially creating a Texture/Surface
    apparition = reflection.coalesce(window);
}

void World::initGLFW() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to Initialize GLFW. Wtf yu durn fam?!\n");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Compute Shaders - Running at 0 fps", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW Window. Why isn't this working?!\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to Initialize GLEW. Fuck!\n");
        return;
    }
}

void World::createChannel() {
    witness = new Receiver(window, instance, shader_program);
    manifestation = new Transmission(apparition, shader_program);
}

void World::logWorkGroupInfo(){
    int workGroupSizes[3] = { 0 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSizes[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSizes[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSizes[2]);
    int workGroupCounts[3] = { 0 };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCounts[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCounts[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCounts[2]);

    std::cout << "Max Compute Workgroup Invocatios: " << GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cout << "Workgroup Size" << workGroupSizes[i] << std::endl;
        std::cout << "Workgroup Count" << workGroupCounts[i] << std::endl;
    }
}

void World::sync() {
    curr_T = glfwGetTime();
    double dt = curr_T - prev_T;

    if (dt > 1) {
        int framerate{ std::max(1, int(n_frames / dt)) };
        std::stringstream title;

        title << "Compute Shaders - Running at " << framerate << " fps";
        glfwSetWindowTitle(window, title.str().c_str());

        prev_T = curr_T;
        n_frames = -1;
        frame_T = float(1000.0 / framerate);
    }

    ++n_frames;
    glClear(GL_COLOR_BUFFER_BIT);
}

void World::persist() {
    prev_T = glfwGetTime();
    n_frames = 0;
    frame_T = 16.0f;

    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    do {
        bool close = witness->update(frame_T/1000.0f);
        if (close) { break; }

        manifestation->update();
        glfwSwapBuffers(window);
        sync();
        glFinish();
    } while (
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0
    );
}