#include "impl.h"


IMPL(minecraft) {
	gl_service service;

	service.on_keydown([](int keycode) {
	});

	service.on_keyup([](int keycode) {
	});

	service.on_display([]() {
	});

	service.run();
}
