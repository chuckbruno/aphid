import os
import sipconfig

# The name of the SIP build file generated by SIP and used by the build
# system.
build_file = "word.sbf"

# Get the SIP configuration information.
config = sipconfig.Configuration()

# Run SIP to generate the code.
os.system(" ".join([config.sip_bin, "-e -c", ".", "-b", build_file, "word.sip"]))

# Create the Makefile.
makefile = sipconfig.SIPModuleMakefile(config, build_file)

# Add the library we are wrapping.  The name doesn't include any platform
# specific prefixes or extensions (e.g. the "lib" prefix on UNIX, or the
# ".dll" extension on Windows).

if config.platform == 'macx-g++':
    makefile.extra_lflags = ['-L../Release']
    makefile.extra_libs = ["wordLibrary"]
else:
    makefile.extra_lflags = ['/NODEFAULTLIB:LIBCMT.lib']
    makefile.extra_libs = ["wordLibrary"]
# Generate the Makefile itself.
makefile.generate()
