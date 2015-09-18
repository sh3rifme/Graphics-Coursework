#pragma comment(lib, "framework.lib")

#include "../Framework/window.h"
#include "Renderer.h"

int main(void) {
	Window w("Coursework", 1920 , 1080, true);

	if(!w.HasInitialised()) {
		return -1;
	}
	
	Renderer renderer(w);
	if(!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1))
			renderer.Toggle(1); 

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2))
			renderer.Toggle(2);

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3))
			renderer.Toggle(3);
		
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4))
			renderer.Toggle(4);
		
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5))
			renderer.Toggle(5);

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_6))
			renderer.Toggle(6);

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R))
			renderer.ResetScene();

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_G))
			renderer.Toggle(10);

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_H))
			renderer.Toggle(11);
		
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_ADD))
			renderer.IncBlur(true);

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SUBTRACT))
			renderer.IncBlur(false);

		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}
	return 0;
}
