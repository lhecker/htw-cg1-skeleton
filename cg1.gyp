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
			'sources': [
				'src/dreiecke.cc',
				'src/dreiecke.fs',
				'src/dreiecke.vs',
				'src/load_shaders.cc',
				'src/load_shaders.h',
				'src/main.cc',
				'src/main.h',
			],
			'copies': [
				{
					'destination': '<(PRODUCT_DIR)',
					'files': [
						'src/dreiecke.fs',
						'src/dreiecke.vs',
					],
				}
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
