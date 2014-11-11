{
	'target_defaults': {
		'default_configuration': 'Debug',
		'configurations': {
			'Debug': {
				'xcode_settings': {
					'COPY_PHASE_STRIP': 'NO',
				},
			},
			'Release': {
				'xcode_settings': {
					'COPY_PHASE_STRIP': 'YES',
				},
			},
		},
	},

	'targets': [
		{
			'target_name': 'cg1',
			'type': 'executable',
			'dependencies': [
				'deps/glew.gyp:libglew',
				'deps/glfw.gyp:libglfw',
			],
			'include_dirs': [
				'deps/glm',
			],
			'sources': [
				'src/gl_service.cc',
				'src/gl_service.h',
				'src/impl.h',
				'src/list.h',
				'src/main.cc',
				'src/minecraft.cc',
				'src/p1a1.cc',
			],
			'conditions': [
				['OS=="mac"', {
					'xcode_settings': {
						'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',
						'CLANG_CXX_LIBRARY': 'libc++',
						'CLANG_ENABLE_OBJC_ARC': 'YES',
					},
					'link_settings': {
						'libraries': [
							'$(SDKROOT)/System/Library/Frameworks/Cocoa.framework',
							'$(SDKROOT)/System/Library/Frameworks/Quartz.framework',
							'$(SDKROOT)/System/Library/Frameworks/QuartzCore.framework',
						],
					},
				}],
			],
		},
	],
}
