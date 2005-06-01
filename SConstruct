env = Environment()
env['CCFLAGS'] = '-Wall'
Export('env')
SConscript(['common/SConscript',
						'light/SConscript',
						'vis/SConscript',
						'qbsp/SConscript',
						'bspinfo/SConscript',
						'qfiles/SConscript',
						'qcc/SConscript',
						'texmake/SConscript',
						'qlumpy/SConscript',
						'sprgen/SConscript',
						'modelgen/SConscript',
], 'env')
