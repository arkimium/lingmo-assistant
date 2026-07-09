cmake_minimum_required(VERSION 3.22)
project(lingmo-assistant VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS
    Core
    Quick
    Qml
    QuickControls2
    Network
    Svg
)

set(LINGMO_SOURCES
    src/main.cpp
    src/core/Message.h
    src/core/Conversation.h
    src/core/Conversation.cpp
    src/core/Settings.h
    src/core/Settings.cpp
    src/core/ContextManager.h
    src/core/ContextManager.cpp
    src/core/TokenEstimator.h
    src/core/TokenEstimator.cpp
    src/core/TrimmingStrategy.h
    src/core/TrimmingStrategy.cpp
        src/core/ThemeProvider.h
    src/core/ThemeProvider.cpp
    src/core/SetupController.h
    src/core/SetupController.cpp
    src/core/UpdateManager.h
    src/core/UpdateManager.cpp
src/core/ChatController.h
    src/core/ChatController.cpp
    src/network/NetworkClient.h
    src/network/NetworkClient.cpp
    src/network/StreamParser.h
    src/network/StreamParser.cpp
    src/providers/AIProvider.h
    src/providers/ProviderManager.h
    src/providers/ProviderManager.cpp
    src/providers/OpenAICompatProvider.h
    src/providers/OpenAICompatProvider.cpp
    src/providers/OllamaProvider.h
    src/providers/OllamaProvider.cpp
    src/providers/LlamaCppProvider.h
    src/providers/LlamaCppProvider.cpp
    src/models/HardwareDetector.h
    src/models/HardwareDetector.cpp
    src/models/ModelDownloader.h
    src/models/ModelDownloader.cpp
    src/models/ModelManager.h
    src/models/ModelManager.cpp
    src/utils/Logger.h
    src/utils/Logger.cpp
    src/utils/Platform.h
    src/utils/Platform.cpp
)

set(LINGMO_RESOURCES
    resources/lingmo-ai-assistant.qrc
)

qt_add_executable(${PROJECT_NAME}
    ${LINGMO_SOURCES}
    ${LINGMO_RESOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickControls2
    Qt6::Network
    Qt6::Svg
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
