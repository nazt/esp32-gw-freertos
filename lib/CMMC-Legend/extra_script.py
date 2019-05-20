from SCons.Script import DefaultEnvironment
import glob, shutil, os

Import("env")

env = DefaultEnvironment()

my_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}
print defines

# env.Replace(PROGNAME="firmware_%s" % defines.get("VERSION"))
# uncomment line below to see environment variables
# env.PIOVERBOSE = 1 
# platform = env.PioPlatform() 
# print env.Dump()
# env.UPLOADERFLAGS = '-v'
# env.Replace(PIOVERBOSE=1)
# ARGUMENTS.Replace(PIOVERBOSE=1)
# print platform
# print ARGUMENTS
if not os.path.exists("lib-dev/CMMC_Legend"):
    os.makedirs("lib-dev/CMMC_Legend")

for file in glob.iglob('src/*.*'):
    print 'Copied file %s' % (file)
    shutil.copy2(file, "lib-dev/CMMC_Legend")

def before_upload(source, target, env):
    print "before_upload"

def after_upload(source, target, env):
    print "after_upload"
    # shutil.rmtree("lib-dev/CMMC_Utils")

def before_build(source, target, env):
    print "before_build" 

def upload_fs(source, target, env):
    print "upload_fs"
    print source 
    print target
    # print env.Dump()



print "Current build targets", map(str, BUILD_TARGETS)

env.AddPreAction("upload", before_upload)
env.AddPreAction("uploadfs", upload_fs)
env.AddPostAction("upload", after_upload)
env.AddPreAction("buildprog", before_build)