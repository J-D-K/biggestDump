#pragma once
#include "Texture.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace SDL
{
    using SharedTexture = std::shared_ptr<SDL::Texture>;

    // JKSV only uses textures. There's no point in making this generic for more types for now, I guess.
    class TextureManager
    {
        public:
            TextureManager(const TextureManager &) = delete;
            TextureManager(TextureManager &&) = delete;
            TextureManager &operator=(const TextureManager &) = delete;
            TextureManager &operator=(TextureManager &&) = delete;

            /*
                Refer to Texture.hpp for how this works completely.
                SDL::SharedTexture Texture = SDL::TextureManager::CreateLoadTexture(TextureName, [Arguments for Texture constructor you're using here.]);
            */
            template <typename... Args>
            static SharedTexture CreateLoadTexture(std::string_view TextureName, Args... Arguments)
            {
                SharedTexture ReturnTexture = nullptr;

                TextureManager &Manager = TextureManager::GetInstance();

                if (Manager.m_TextureMap.find(TextureName.data()) != Manager.m_TextureMap.end() &&
                    !Manager.m_TextureMap.at(TextureName.data()).expired())
                {
                    ReturnTexture = Manager.m_TextureMap.at(TextureName.data()).lock();
                }
                else
                {
                    ReturnTexture = std::make_shared<SDL::Texture>(Arguments...);

                    Manager.m_TextureMap[TextureName.data()] = ReturnTexture;
                }
                return ReturnTexture;
            }

        private:
            // No constructing.
            TextureManager(void) = default;
            static TextureManager &GetInstance(void)
            {
                static TextureManager Instance;
                return Instance;
            }
            // Map of weak_ptrs to textures so they free automatically after they expire.
            static inline std::unordered_map<std::string, std::weak_ptr<SDL::Texture>> m_TextureMap;
    };
} // namespace SDL
