#include "../../Scene/SceneManager.h"
 
class MainScene : public Engine::Scene {
public:
    MainScene()
        : Scene("MainScene") {
    }

    void Initialize() override;
};