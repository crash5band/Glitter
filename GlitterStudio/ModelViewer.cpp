#include "ModelViewer.h"
#include "UiHelper.h"
#include "Editor.h"

ModelViewer::ModelViewer() :
	model{ nullptr }, shader{ nullptr }, camWindowOpen{ false }
{
	framebuffer = new RenderTarget(1920, 1080);
	camera = new Camera(CameraMode::Normal, glm::vec3(0, 0.5, 3));
	light = new Light();
}

ModelViewer::~ModelViewer()
{
	delete framebuffer;
	delete camera;
	delete light;
}

void ModelViewer::setModel(ModelData* model)
{
	this->model = model;
}

void ModelViewer::setShader(Shader* shader)
{
	this->shader = shader;
}

ModelData* ModelViewer::activeModel() const
{
	return model;
}

Shader* ModelViewer::activeShader() const
{
	return shader;
}

RenderTarget* ModelViewer::getFramebuffer() const
{
	return framebuffer;
}

glm::mat4 ModelViewer::getProjectionMatrix() const
{
	return camera->getProjectionMatrix(1);
}

void ModelViewer::updatePreview()
{
	framebuffer->use();
	if (model && shader)
	{
		shader->setVec3("light.direction", light->direction);
		shader->setVec4("light.color", light->color);
		shader->setFloat("light.ambient", light->ambient);
		shader->setFloat("light.diffuse", light->diffuse);
		shader->setFloat("light.specular", light->specular);
		shader->setVec3("viewPos", camera->getPosition());
		shader->setMatrix4("view", camera->getViewMatrix());
		shader->setMatrix4("projection", camera->getProjectionMatrix(ImGui::GetWindowSize().x / (ImGui::GetWindowSize().y - 150.0f)));
		model->draw(shader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	auto texture = framebuffer->getTexture();
	viewArea = ImRect(ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowSize().x,
		ImGui::GetCursorScreenPos().y + ImGui::GetWindowSize().y - 150.0f));

	mouseInViewArea = ImGui::IsMouseHoveringRect(viewArea.Min, viewArea.Max);
	if (mouseInViewArea && ImGui::IsWindowFocused())
	{
		camera->update(ImGui::GetIO());
	}

	ImGui::Image((void*)texture, ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 150.0f), ImVec2(1, 1), ImVec2(0, 0));
}

void ModelViewer::update()
{
	if (camWindowOpen)
		camera->updateUI();

	if (!Editor::editorWindows.modelViewerOpen)
		return;

	if (ImGui::Begin("Model Viewer", &Editor::editorWindows.modelViewerOpen))
	{
		updatePreview();
	}

	ImGui::End();
}