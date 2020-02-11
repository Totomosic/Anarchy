#include "Bolt.h"

namespace Anarchy
{

	class WorldEditor : public Application
	{
	private:
		std::unique_ptr<RenderTexture2D> m_RenderTexture;

	public:
		void Init() override
		{
			float sceneWidth = 900;
			float sceneHeight = Height() - 40;
			GetWindow().SetClearColor(Color(225, 225, 225));

			Scene& scene = SceneManager::Get().AddScene();
			EntityHandle camera = scene.GetFactory().Camera(Matrix4f::Orthographic(0, sceneWidth, 0, sceneHeight, -100, 100));
			Layer& layer = scene.AddLayer();
			layer.SetActiveCamera(camera);

			EntityHandle uiCamera = scene.GetFactory().Camera(Matrix4f::Orthographic(0, Width(), 0, Height(), -100, 100));
			Layer& uiLayer = scene.AddLayer();
			uiLayer.SetActiveCamera(uiCamera);

			m_RenderTexture = std::make_unique<RenderTexture2D>(sceneWidth, sceneHeight);
			layer.GetRenderer().SetRenderTarget(m_RenderTexture.get());

			layer.GetFactory().Rectangle(300, 300, Color::Red, Transform({ 300, 300, 0 }));

			UIElement& root = uiLayer.GetUI().GetRoot();
			UIRectangle& window = root.CreateImage(sceneWidth, sceneHeight, m_RenderTexture.get(), Transform({ sceneWidth / 2.0f + 20, Height() / 2.0f, 0 }));
			UIElement& panel = root.CreateElement(Transform({ sceneWidth + 20 * 2 + (Width() - sceneWidth - 20 * 3) / 2.0f, Height() / 2.0f, 0 }));

			panel.CreateText("Options", ResourceManager::Get().Fonts().Calibri(18), Color::Black, Transform({ 0, 300, 1 }));
		}

		void Update() override
		{

		}

		void Render() override
		{
			m_RenderTexture->Clear();
			Graphics::Get().RenderScene();
		}
	};

}

int main()
{
	EngineCreateInfo info;
	info.WindowInfo.Title = "World Editor";
	Engine e(info);
	e.SetApplication<Anarchy::WorldEditor>();
	e.Run();
	return 0;
}