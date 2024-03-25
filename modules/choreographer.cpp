#include "./choreographer.h"

World::World() { initGLFW(); }
World::~World() { 
    for (unsigned int& shader : shader_modules) {
        glDeleteProgram(shader);
    }

    glDeleteTextures(1, &apparition);

    delete manifestation;
    delete voyager;

    glfwTerminate();
}

void World::persist() {
    prev_T = glfwGetTime();
    n_frames = 0;
    frame_T = 16.0f;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    do {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0
    );
}

void World::initGL() {
    initShaders();

    glClearColor(0.1f, 0.3f, 0.23f, 1.0f);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glDisable(GL_DEPTH_TEST);

    Reflection reflection;  // This is essentially creating a Texture/Surface
    apparition = reflection.coalesce(window);
}

void World::initGLFW() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to Initialize GLFW. Wtf yu durn fam?!\n");
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Back At the OpenGL", NULL, NULL);

    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW Window. Why isn't this working?!\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to Initialize GLEW. Fuck!\n");
        return;
    }
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