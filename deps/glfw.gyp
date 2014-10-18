{
	'targets': [
		{
			'target_name': 'libglfw',
			'type': 'static_library',
			'include_dirs': [
				'glfw/src',
			],
			'sources': [
				'glfw/src/context.c',
				'glfw/src/init.c',
				'glfw/src/input.c',
				'glfw/src/monitor.c',
				'glfw/src/window.c',
			],
			'defines': [
				'_GLFW_USE_OPENGL=1',
				'GLFW_VERSION_MAJOR=3',
				'GLFW_VERSION_MINOR=1',
				'GLFW_VERSION_PATCH=0',
				'GLFW_VERSION_EXTRA=',
				'GLFW_VERSION=3.1',
				'GLFW_VERSION_FULL=3.1.0',
			],
			'conditions': [
				['OS=="linux"', {
					'sources': [
						'glfw/src/glx_context.c',
						'glfw/src/linux_joystick.c',
						'glfw/src/posix_time.c',
						'glfw/src/posix_tls.c',
						'glfw/src/x11_init.c',
						'glfw/src/x11_monitor.c',
						'glfw/src/x11_window.c',
						'glfw/src/xkb_unicode.c',
					],
					'defines': [
						'_GLFW_GLX=1',
						'_GLFW_X11=1',
					],
					'link_settings': {
						'libraries': [
							'-lX11',
							'-lXrandr',
							'-lXinerama',
							'-lXinput',
							'-lXxf86vm',
							'-lXkb',
							'-lXcursor',
							'-lrt',
							'-lm',
						],
					},
				}],
				['OS=="mac"', {
					'sources': [
						'glfw/src/cocoa_init.m',
						'glfw/src/cocoa_monitor.m',
						'glfw/src/cocoa_window.m',
						'glfw/src/iokit_joystick.m',
						'glfw/src/mach_time.c',
						'glfw/src/nsgl_context.m',
						'glfw/src/posix_tls.c',
					],
					'defines': [
						'_GLFW_COCOA=1',
						'_GLFW_NSGL=1',
						'_GLFW_USE_CHDIR=1',
						'_GLFW_USE_MENUBAR=1',
						'_GLFW_USE_RETINA=1',
					],
					'link_settings': {
						'libraries': [
							'$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
							'$(SDKROOT)/System/Library/Frameworks/OpenGL.framework',
							'$(SDKROOT)/System/Library/Frameworks/IOKit.framework',
							'$(SDKROOT)/System/Library/Frameworks/CoreFoundation.framework',
							'$(SDKROOT)/System/Library/Frameworks/CoreVideo.framework',
						],
					},
				}],
				['OS=="win"', {
					'sources': [
						'glfw/src/win32_init.c',
						'glfw/src/win32_monitor.c',
						'glfw/src/win32_time.c',
						'glfw/src/win32_tls.c',
						'glfw/src/win32_window.c',
						'glfw/src/winmm_joystick.c',
						'glfw/src/wlx_context.c',
					],
					'defines': [
						'_GLFW_USE_DWM_SWAP_INTERVAL=1',
						'_GLFW_WGL=1',
						'_GLFW_WIN32=1',
						'DWINVER=0x0501',
						'UNICODE',
					],
					'link_settings': {
						'libraries': [
							'-lopengl32',
						],
					},
				}],
			],
			'direct_dependent_settings': {
				'include_dirs': [
					'glfw/include',
				],
			},
		}
	],
}
