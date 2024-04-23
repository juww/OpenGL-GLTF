#include "renderer.h"

#include "GUI.h"

Renderer* Renderer::instance = nullptr;

GUI::TerrainParam tp(4, 27.9f, 4, 0.5f, 2.0f, {0.0f, 0.0f}, 5.0f);
GUI::GrassParam gp(3.0f, 0.5f, 1.12f, 0.7f);

Renderer::Renderer() {
    m_Camera = new Camera(glm::vec3(0.0f, 0.0f, 5.0f));
    m_Lights.clear();
    m_Shaders.clear();
    m_Skybox = new Skybox();
    m_Plane = nullptr;
    m_LightCube = nullptr;
}

Renderer::~Renderer() {

}

Renderer* Renderer::getInstance() {
    if (instance == nullptr) {
        instance = new Renderer();
    }
    return instance;
}

Camera* Renderer::getCamera() {
    return m_Camera;
}


void Renderer::configureGlobalState() {
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_FRAMEBUFFER_SRGB); // gamma correction 
}

void Renderer::setupLights() {
    Light l1;
    l1.setLight(glm::vec3(-0.2f, -1.0f, -0.3f));
    m_Lights.push_back(l1);
    l1.setLight(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f, 1.0f);
    m_Lights.push_back(l1);
}

void Renderer::setupShaders() {

    // build and compile our shader zprogram
    // ------------------------------------
    m_ModelShader = new Shader("cube.vs", "cube.fs", "normalMapping.gs");
    m_PlaneShader = new Shader("plane.vs", "plane.fs");
    m_GrassShader = new Shader("grass.vs", "grass.fs");
    m_LightCubeShader = new Shader("light_cube.vs", "light_cube.fs");
    m_SkyboxShader = new Shader("skybox.vs", "skybox.fs");
    // planeShader.setTessellationShader("TessellationControlShader.tcs", "TessellationEvaluationShader.tes");
    //Shader skeletalModel("skeletal.vs", "skeletal.fs", "skeletal.gs");
}

void Renderer::initModel() {
    const std::string& pathfile = "res/models/simpleSkin/scene.gltf";
    //const std::string& pathfile = "assets/models/phoenix_bird/scene.gltf";
    m_Model = new loadModel(pathfile.c_str());

    m_Model->animator.doAnimation(0);
}

void Renderer::start() {
    configureGlobalState();
    setupLights();
    setupShaders();

    initModel();

    m_Plane = new Plane(65);
    m_Plane->InitTerrainChunk(1, 64.0f, m_Camera->Position);
    m_Plane->initGrass(50);
    m_Plane->setAllUniform(m_PlaneShader);
    m_Plane->grass.generateNoiseMap(m_GrassShader, 1, 10.0f, 4, 1.5f, 2.0f, { 0.0f,0.0f }); 

    m_LightCube = new Cube();
    m_LightCube->initialize();

    m_Skybox = new Skybox();
}
//nanti dipindahin ke class model

void Renderer::setModelShader(const glm::mat4& projection, const glm::mat4& view) {
    m_ModelShader->use();
    int light_n = m_Lights.size();
    m_ModelShader->setInt("light_n", light_n);
    for (int i = 0; i < light_n; i++) {
        m_ModelShader->setInt("lightID[" + std::to_string(i) + "]", i + 1);
        m_ModelShader->setVec3("lightDirection[" + std::to_string(i) + "]", m_Lights[i].m_Direction);
        m_ModelShader->setVec3("lightPosition[" + std::to_string(i) + "]", m_Lights[i].m_Position);
        m_ModelShader->setVec3("lightColor[" + std::to_string(i) + "]", m_Lights[i].m_Color);
    }

    m_ModelShader->setVec3("viewPos", m_Camera->Position);
    m_ModelShader->setMat4("projection", projection);
    m_ModelShader->setMat4("view", view);

}

void Renderer::render(float currentTime) {

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(m_Camera->Zoom), m_Camera->Aspect, 0.1f, 100.0f);
    glm::mat4 view = m_Camera->GetViewMatrix();

    setModelShader(projection, view);
    m_Model->update(m_ModelShader, currentTime);
    m_Model->DrawModel(m_ModelShader);

    bool changeParam = GUI::proceduralTerrainParam(tp.m_Seed, tp.m_Scale, tp.m_Octaves, tp.m_Persistence, tp.m_Lacunarity, tp.m_OffsetV, tp.m_Amplitude);
    GUI::grassParam(gp.m_Frequency, gp.m_Amplitude, gp.m_Scale, gp.m_Drop);

    // draw plane
    m_Plane->update(m_Camera->Position, tp.m_Seed, tp.m_Scale, tp.m_Octaves, tp.m_Persistence, tp.m_Lacunarity, tp.m_OffsetV, tp.m_Amplitude, changeParam);
    m_Plane->draw(m_PlaneShader, projection, view);
    m_Plane->drawGrass(m_GrassShader, projection, view, currentTime, gp.m_Frequency, gp.m_Amplitude, gp.m_Scale, gp.m_Drop);

    m_LightCube->draw(m_LightCubeShader, projection, view);

    m_Skybox->draw(m_SkyboxShader, projection, glm::mat4(glm::mat3(m_Camera->GetViewMatrix())));
}