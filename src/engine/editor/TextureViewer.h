// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2017-2022 TalonBrave.info and Others (see CONTRIBUTORS)

#include "BaseWindow.h"

class TextureViewer : public BaseWindow {
public:
	explicit TextureViewer( ohw::TextureResource *texture ) : BaseWindow() {
		texturePtr = texture;
		texturePtr->AddReference();
	}

	explicit TextureViewer( const std::string &path ) {
		texturePtr = ohw::GetApp()->resourceManager->LoadTexture( path, ohw::TextureResource::FLAG_NOMIPS );
		if ( texturePtr == nullptr ) {
			throw std::runtime_error( "Failed to load specified texture, \"" + path + "\" (" + PlGetError() + ")!" );
		}

		texturePath = path;
	}

	~TextureViewer() override {
		if ( texturePtr != nullptr ) {
			texturePtr->Release();
		}
	}

	void ReloadTexture( PLGTextureFilter filter_mode ) {
		if ( texturePath.empty() || filter_mode == filterMode ) {
			return;
		}

		texturePtr->Release();
		ohw::GetApp()->resourceManager->ClearAllResources();

		texturePtr = ohw::GetApp()->resourceManager->LoadTexture( texturePath, filter_mode );
		filterMode = filter_mode;
	}

	void Display() override {
		unsigned int texWidth = texturePtr->GetWidth();
		unsigned int texHeight = texturePtr->GetHeight();

		ImGui::SetNextWindowSize( ImVec2( texWidth + 64, texHeight + 128 ), ImGuiCond_Once );
		Begin( "Texture Viewer",
			   ImGuiWindowFlags_MenuBar |
				   ImGuiWindowFlags_HorizontalScrollbar |
				   ImGuiWindowFlags_NoSavedSettings
		);

		if ( ImGui::BeginMenuBar() ) {
			if ( ImGui::BeginMenu( "View" ) ) {
				ImGui::SliderInt( "Scale", &scale_, 1, 8 );
				if ( ImGui::BeginMenu( "Filter Mode" ) ) {
					if ( ImGui::MenuItem( "Linear", nullptr, ( filterMode == PLG_TEXTURE_FILTER_LINEAR ) ) ) {
						ReloadTexture( PLG_TEXTURE_FILTER_LINEAR );
					}
					if ( ImGui::MenuItem( "Nearest", nullptr, ( filterMode == PLG_TEXTURE_FILTER_NEAREST ) ) ) {
						ReloadTexture( PLG_TEXTURE_FILTER_NEAREST );
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		unsigned int internalId = texturePtr->GetInternalTexture()->internal.id;
		ImGui::Image( reinterpret_cast<ImTextureID>( internalId ), ImVec2( texWidth * scale_, texHeight * scale_ ) );
		ImGui::Separator();
		ImGui::Text( "Path: %s", texturePath.c_str() );
		ImGui::Text( "%dx%d", texWidth, texHeight );
		size_t texSize = texturePtr->GetTextureSize();
		ImGui::Text( "Size: %ukB (%luB)", ( unsigned int ) PlBytesToKilobytes( texSize ), ( long unsigned ) texSize );

		ImGui::End();
	}

protected:
private:
	ohw::TextureResource *texturePtr{ nullptr };
	std::string texturePath;

	PLGTextureFilter filterMode{ PLG_TEXTURE_FILTER_LINEAR };

	int scale_{ 1 };
};
