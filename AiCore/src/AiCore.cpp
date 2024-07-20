#include <AiCore.h>
#include <memory>

namespace Ai {
    GLFWwindow* window;

    static unsigned int SRC_WIDTH = 1000;
    static unsigned int SRC_HEIGHT = 1000;

    static float WH_Ratio = float(SRC_WIDTH) / float(SRC_HEIGHT);

    struct PainterObject {
        unsigned int VAO;
        unsigned int VBO;
    };

    PainterObject g_triangle;
    PainterObject g_square;
    PainterObject g_line;

    float lineVertices[6] = { 1.0f };

    static float triangleVertices[9] = {
         0.0f, 0.1f, 1.0f,
         0.1f,-0.1f, 1.0f,
        -0.1f,-0.1f, 1.0f
    };

    static float squareVertices[18] = {
        -0.1f, 0.1f, 1.0f,
         0.1f, 0.1f, 1.0f,
         0.1f,-0.1f, 1.0f,

        -0.1f, 0.1f, 1.0f,
        -0.1f,-0.1f, 1.0f,
         0.1f,-0.1f, 1.0f,
    };

    std::vector<std::shared_ptr<Painter>> RenderObjectVector;

    std::map<std::string, unsigned int> ShaderMap;

    void renderAiInit() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "HelloWindow~", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(1);
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            exit(1);
        }

        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        g_triangle.VAO = VAO;
        g_triangle.VBO = VBO;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        g_square.VAO = VAO;
        g_square.VBO = VBO;

        addLineShader();
        addCanvasShader();
    }

    void renderAi()
    {
        while (!glfwWindowShouldClose(window))
        {
            if (RenderObjectVector.size() != 0) {
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                int vertexScaleLocation;
                int vertexPosLocation;
                int vertexColorLocation;

                for (int i = 0; i < RenderObjectVector.size(); i++) {
                    std::pair<float, float> point;
                    std::pair<float, float> scale;
                    std::pair<float, float> pos;
                    std::tuple<float, float, float> color;

                    switch (RenderObjectVector[i]->getObjectType()) {
                    case ObjectType::LINE:
                        glUseProgram(ShaderMap["Line"]);
                        glLineWidth(1.0f);
                        point = std::dynamic_pointer_cast<Line>(RenderObjectVector[i])->getStartPoint();
                        lineVertices[0] = point.first;
                        lineVertices[1] = point.second;
                        point = std::dynamic_pointer_cast<Line>(RenderObjectVector[i])->getEndPoint();
                        lineVertices[3] = point.first;
                        lineVertices[4] = point.second;
                        glGenVertexArrays(1, &g_line.VAO);
                        glGenBuffers(1, &g_line.VBO);
                        glBindVertexArray(g_line.VAO);
                        glBindBuffer(GL_ARRAY_BUFFER, g_line.VBO);
                        glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                        glEnableVertexAttribArray(0);
                        color = std::dynamic_pointer_cast<Line>(RenderObjectVector[i])->getColor();
                        vertexColorLocation = glGetUniformLocation(ShaderMap["Line"], "ourColor");
                        glUniform4f(vertexColorLocation, std::get<0>(color), std::get<1>(color), std::get<2>(color), 1.0f);
                        glDrawArrays(GL_LINES, 0, 2);
                        glDeleteVertexArrays(1, &g_line.VAO);
                        glDeleteBuffers(1, &g_line.VBO);
                        glBindBuffer(GL_ARRAY_BUFFER, 0);
                        glBindVertexArray(0);
                        break;
                    case ObjectType::TRIANGLE:
                        glUseProgram(ShaderMap["Canvas"]);
                        glBindVertexArray(g_triangle.VAO);
                        vertexScaleLocation = glGetUniformLocation(ShaderMap["Canvas"], "scale");
                        scale = std::dynamic_pointer_cast<Triangle>(RenderObjectVector[i])->getScale();
                        glUniform2f(vertexScaleLocation, scale.first, scale.second * WH_Ratio);
                        pos = std::dynamic_pointer_cast<Triangle>(RenderObjectVector[i])->getPosition();
                        vertexPosLocation = glGetUniformLocation(ShaderMap["Canvas"], "pos");
                        glUniform2f(vertexPosLocation, pos.first, pos.second);
                        color = std::dynamic_pointer_cast<Triangle>(RenderObjectVector[i])->getColor();
                        vertexColorLocation = glGetUniformLocation(ShaderMap["Canvas"], "ourColor");
                        glUniform4f(vertexColorLocation, std::get<0>(color), std::get<1>(color), std::get<2>(color), 1.0f);
                        glDrawArrays(GL_TRIANGLES, 0, 3);
                        break;
                    case ObjectType::SQUARE:
                        glUseProgram(ShaderMap["Canvas"]);
                        glBindVertexArray(g_square.VAO);
                        vertexScaleLocation = glGetUniformLocation(ShaderMap["Canvas"], "scale");
                        scale = std::dynamic_pointer_cast<Square>(RenderObjectVector[i])->getScale();
                        glUniform2f(vertexScaleLocation, scale.first, scale.second * WH_Ratio);
                        pos = std::dynamic_pointer_cast<Square>(RenderObjectVector[i])->getPosition();
                        vertexPosLocation = glGetUniformLocation(ShaderMap["Canvas"], "pos");
                        glUniform2f(vertexPosLocation, pos.first, pos.second);
                        color = std::dynamic_pointer_cast<Square>(RenderObjectVector[i])->getColor();
                        vertexColorLocation = glGetUniformLocation(ShaderMap["Canvas"], "ourColor");
                        glUniform4f(vertexColorLocation, std::get<0>(color), std::get<1>(color), std::get<2>(color), 1.0f);
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                        break;
                    }
                }
                glfwSwapBuffers(window);
                glfwPollEvents();
            }
            else {
                std::cout << "Nothing to rendering~" << std::endl;
                break;
            }
        }
        glDeleteVertexArrays(1, &g_triangle.VAO);
        glDeleteBuffers(1, &g_triangle.VBO);
        glDeleteVertexArrays(1, &g_square.VAO);
        glDeleteBuffers(1, &g_square.VBO);
        // TODO::Delete shader programs...
    }

    void addLineShader()
    {
        const char* vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";
        const char* fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec4 ourColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = ourColor;\n"
            "}\n\0";
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        ShaderMap["Line"] = shaderProgram;
    }

    void addCanvasShader()
	{
        const char* vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "uniform vec2 pos;\n"
            "uniform vec2 scale;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x * scale.x, aPos.y * scale.y, aPos.z, 1.0) + vec4(pos, 0.0, 0.0);\n"
            "}\0";
        const char* fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec4 ourColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = ourColor;\n"
            "}\n\0";
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        int  success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        unsigned int shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        ShaderMap["Canvas"] = shaderProgram;
	}

    void addLine(unsigned int id, float startPointX, float startPointY, float endPointX, float endPointY) {
        RenderObjectVector.push_back(std::make_shared<Line>(id, startPointX, startPointY, endPointX, endPointY));
    }

    void addLine(unsigned int id, float startPointX, float startPointY, float endPointX, float endPointY,
        float red, float green, float blue) {
        RenderObjectVector.push_back(std::make_shared<Line>(id, startPointX, startPointY, endPointX, endPointY, red, green, blue));
    }

    void addTriangle(unsigned int id, float xscale, float yscale, float xpos, float ypos) {
        RenderObjectVector.push_back(std::make_shared<Triangle>(id, xscale, yscale, xpos, ypos));
    }

    void addTriangle(unsigned int id, float xscale, float yscale, float xpos, float ypos, float red, float green, float blue) {
        RenderObjectVector.push_back(std::make_shared<Triangle>(id, xscale, yscale, xpos, ypos, red, green, blue));
    }

    void addSquare(unsigned int id, float xscale, float yscale, float xpos, float ypos) {
        RenderObjectVector.push_back(std::make_shared<Square>(id, xscale, yscale, xpos, ypos));
    }

    void addSquare(unsigned int id, float xscale, float yscale, float xpos, float ypos, float red, float green, float blue) {
        RenderObjectVector.push_back(std::make_shared<Square>(id, xscale, yscale, xpos, ypos, red, green, blue));
    }
}