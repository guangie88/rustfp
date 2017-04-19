#!/usr/bin/env python
# encoding: utf-8

#
# required global variables
#

# common imports
import os, sys

# standard settings for application name and version
APPNAME = 'rustfp'
VERSION = '1.0'

# platform checks
PLATFORM_ALL = 'all'
PLATFORM_LINUX = 'linux'
PLATFORM_WINDOWS = 'win32'
PLATFORM_CYGWIN = 'cygwin'
PLATFORM_OTHERS = 'others'

IS_PLATFORM_LINUX = sys.platform.startswith(PLATFORM_LINUX)
IS_PLATFORM_WINDOWS = sys.platform.startswith(PLATFORM_WINDOWS)
IS_PLATFORM_CYGWIN = sys.platform.startswith(PLATFORM_CYGWIN)
IS_PLATFORM_OTHERS = not (IS_PLATFORM_LINUX or IS_PLATFORM_WINDOWS or IS_PLATFORM_CYGWIN)

STATIC_FLAG = 'static_flag'
SHLIB_MARKER = 'SHLIB_MARKER'

# platform configuration variable key
LIB_KEY = 'lib'
LIBPATH_KEY = 'libpath'
LINKFLAGS_KEY = 'linkflags'
CXXFLAGS_KEY = 'cxxflags'
INCLUDES_KEY = 'includes'
DEFINES_KEY = 'defines'

# build configuration enumerations
BLD_CONF_COMMON = 'common'
BLD_CONF_DEBUG = 'debug'
BLD_CONF_RELEASE = 'release'

# context command strings
BUILD_COMMAND = 'build'
CLEAN_COMMAND = 'clean'
INSTALL_COMMAND = 'install'
UNINSTALL_COMMAND = 'uninstall'
LIST_COMMAND = 'list'
STEP_COMMAND = 'step'

# option to change compiler for all platforms
from waflib.Tools.compiler_cxx import cxx_compiler
cxx_compiler[PLATFORM_LINUX] = [ 'g++' ]
cxx_compiler[PLATFORM_WINDOWS] = [ 'msvc' ]
cxx_compiler[PLATFORM_CYGWIN] = [ 'g++' ]

#
# user defined editables
#

# binary designated directories
bin_project_paths = [ 'unit_test' ]

# directories excluded from building
# this takes precedence over bin_project_paths
# uses regex search pattern for flexibility
# remember to add begin anchor (^) + end anchor ($) to match entire string instead
# empty strings will be skipped
excluded_dir_paths = []

# options for global settings
# just following the key value pair structure from PLATFORM_ALL : BLD_CONF_COMMON
# by default, src_dir is in include paths and lib_dir is in library paths
global_options_map = {
    PLATFORM_ALL: {
        BLD_CONF_COMMON: {
        },

        BLD_CONF_DEBUG: {
        },

        BLD_CONF_RELEASE: {
        }
    },

    PLATFORM_LINUX: {
        BLD_CONF_COMMON: {
            LIB_KEY: [],
            LIBPATH_KEY: [],
            LINKFLAGS_KEY: [],
            CXXFLAGS_KEY: [ '-std=c++14' ],
            INCLUDES_KEY: [],
            DEFINES_KEY: []
        },

        BLD_CONF_DEBUG: {
        },

        BLD_CONF_RELEASE: {
        }
    },

    PLATFORM_WINDOWS: {
        BLD_CONF_COMMON: {
        },

        BLD_CONF_DEBUG: {
        },

        BLD_CONF_RELEASE: {
        }
    },

    PLATFORM_CYGWIN: {
        BLD_CONF_COMMON: {
            CXXFLAGS_KEY: [ '-std=c++14' ]
        },

        BLD_CONF_DEBUG: {
        },

        BLD_CONF_RELEASE: {
        }
    },

    PLATFORM_OTHERS: {
        BLD_CONF_COMMON: {
        },

        BLD_CONF_DEBUG: {
        },

        BLD_CONF_RELEASE: {
        }
    }
}

# settings for individual project
# the format follows that of global settings
project_options_map = {
    'unit_test': {
        PLATFORM_LINUX: {
            BLD_CONF_COMMON: {
                LIB_KEY: [ 'gtest' ],
                LIBPATH_KEY: [],
                LINKFLAGS_KEY: [],
                CXXFLAGS_KEY: [],
                INCLUDES_KEY: [],
                DEFINES_KEY: []
            },

            BLD_CONF_DEBUG: {
            },

            BLD_CONF_RELEASE: {
            }
        }
    }
}

#
# end of user defined editable
#

# other standard constants
BLD_CONF_KEY = 'build_config'
COMPILER_CXX_KEY = 'compiler_cxx'
TARGET_SUFFIX_KEY = 'target_suffix'
USE_ENV_KEY = 'use'
DEBUG_DEFINE_VALUE = 'DEBUG'
RELEASE_DEFINE_VALUE = 'RELEASE'
GLOBAL_SETTINGS = 'GLOBAL_SETTINGS'

# standard default flags for common, debug and release modes
if IS_PLATFORM_LINUX:
    common_cxxflags = [ '-Wall' ]
    debug_cxxflags = [ '-g', '-O0' ]
    release_cxxflags = [ '-O3' ]
    common_ldflags = []
    debug_ldflags = []
    release_ldflags = []
    static_lib_suffix = ''
elif IS_PLATFORM_WINDOWS:
    common_cxxflags = [ '/W4' ]
    debug_cxxflags = [ '/Od', '/Z7' ]
    release_cxxflags = [ '/Ox' ]
    common_ldflags = []
    debug_ldflags = []
    release_ldflags = []
    static_lib_suffix = '-static'
elif IS_PLATFORM_CYGWIN:
    common_cxxflags = [ '-Wall' ]
    debug_cxxflags = [ '-g', '-O0' ]
    release_cxxflags = [ '-O3' ]
    common_ldflags = []
    debug_ldflags = []
    release_ldflags = []
    static_lib_suffix = '-static'
else:
    common_cxxflags = [ '-Wall' ]
    debug_cxxflags = [ '-g', '-O0' ]
    release_cxxflags = [ '-O3' ]
    common_ldflags = []
    debug_ldflags = []
    release_ldflags = []
    static_lib_suffix = ''

# waf settings for top and out directories 
top = '.'
out = 'build'

# directory names
src_dir = 'src'
obj_dir = 'obj'
lib_dir = 'lib'
bin_dir = 'bin'

dbg_dir = BLD_CONF_DEBUG
rel_dir = BLD_CONF_RELEASE
default_bld_dir = rel_dir

# extensions
src_exts = [ '.cpp', '.cxx' ]

# constants
RPATH_ORIGIN = '$ORIGIN'
RPATHS_PROGRAM = RPATH_ORIGIN + ':' + RPATH_ORIGIN + '/../' + lib_dir
RELEASE_SUFFIX = ''
DEBUG_SUFFIX = '-debug'
PREFIX_PATH = '/usr/local'
INSTALL_BIN_PATH = PREFIX_PATH + '/bin'
INSTALL_LIB_PATH = PREFIX_PATH + '/lib'


def generate_platforms_used(platform):
    # common platform to be included first
    platforms_used = [ PLATFORM_ALL ]

    supported_platforms = [ PLATFORM_LINUX, PLATFORM_WINDOWS, PLATFORM_CYGWIN ]
    is_other_platform = True

    for supported_platform in supported_platforms:
        if platform.startswith(supported_platform):
            platforms_used.append(supported_platform)
            is_other_platform = False

    if is_other_platform:
        platforms_used.append(PLATFORM_OTHERS)

    return platforms_used


def generate_build_configs_used(build_config):
    # common build configuration to be included first
    build_configs_used = [ BLD_CONF_COMMON, build_config ]
    return build_configs_used


def remove_first_dir_from_path(dir_path):
    dir_parts = []
    parent_path = dir_path

    while True:
        # splits into parent and child paths
        parent_path, child_path = os.path.split(parent_path)

        if parent_path:
            dir_parts.insert(0, child_path)
        else:
            break

    # performs splat operator to split a list into separate arguments
    if dir_parts:
        return os.path.join(*dir_parts)
    else:
        return ''


def match_excluded_dir_path(target, excluded_dir_paths):
    from re import search 

    # only process non-empty strings
    for excluded_dir_path in excluded_dir_paths:
        if excluded_dir_path and search(excluded_dir_path, target):
            return True

    return False


def merge_platforms_build_configs_options(conf, platforms_map):
    merged_options_map = {}

    platforms_used = generate_platforms_used(sys.platform)
    build_configs_used = generate_build_configs_used(conf.env[BLD_CONF_KEY])

    for platform_key in platforms_used:
        build_configs_map = platforms_map.get(platform_key)

        if build_configs_map:
            for build_config_key in build_configs_used:
                options_map = build_configs_map.get(build_config_key)

                if options_map:
                    for option_key in options_map:
                        option = options_map.get(option_key)

                        # standardize for option to always be in a list
                        if not isinstance(option, list):
                            option = [ option ]

                        # if option key already exists then just append
                        if option_key in merged_options_map:
                            merged_options_map[option_key] += option
                        else:
                            merged_options_map[option_key] = option

    return merged_options_map


def canonicalize_library_paths(libpaths):
    canonical_libpaths = []

    # check if libpaths is single value
    # if so, standardize to list
    if not isinstance(libpaths, list):
        libpaths = [ libpaths ]

    for libpath in libpaths:
        canonical_libpaths.append(os.path.join(os.getcwd(), libpath))

    return canonical_libpaths


# project can be some predefined string which represents global settings
def add_options_to_env(conf, project, platforms_map):
    merged_attributes_map = merge_platforms_build_configs_options(conf, platforms_map)

    for attribute_key in merged_attributes_map:
        if project == GLOBAL_SETTINGS:
            # this is for global attribute_key set
            attribute_env_key = attribute_key.upper()
        else:
            attribute_env_key = attribute_key.upper() + '_' + project + '_' + USE_ENV_KEY

        attribute_value = merged_attributes_map.get(attribute_key)

        # process the LIB_KEY attribute_key last
        if not attribute_key == LIB_KEY:
            # library paths need to be full and canonical
            if attribute_key == LIBPATH_KEY:
                attribute_value = canonicalize_library_paths(attribute_value)

            if not conf.env[attribute_env_key]:
                conf.env[attribute_env_key] = attribute_value
            else:
                conf.env[attribute_env_key] += attribute_value

    merged_lib_list = merged_attributes_map.get(LIB_KEY)

    if merged_lib_list:
        check_and_add_libraries(conf, project, merged_lib_list)


def check_and_add_libraries(conf, project, libraries):
    # splits the linking libraries into local and third-party ones
    # local is defined to be those library files derived from compilation of local files
    # only the third-party ones would have to be checked and used later
    project_use_key = project + '_' + USE_ENV_KEY

    # confirm that the libraries contain a list of linking library strings
    # if just a single value, convert it into a list
    if not isinstance(libraries, list):
        libraries = [ libraries ]

    for library in libraries:
        # if library is just a single file name, then library_full_str == library
        library_full_str = library

        # the library may come in a form of library group
        # that is a string with multiple libraries delimited by space
        # or grouped in a list []
        # so that the check_cxx can test both the libraries in the same test
        # waf rejects shared library with unreferenced symbols and therefore
        # sometimes there is a need to test a group of libraries together

        # library_full_str joins up all libraries (may be just a single library)
        # with space delimiter
        # this is required because check_cxx only accepts a single string syntax
        if isinstance(library, list):
            library_full_str = ' '.join(map(str, library))

        # split the joined string back into a confirmed list of library files
        library_grp = library_full_str.split()

        # if the library matches any of the project names
        # and does not match any of the excluded directory paths
        # then it is considered to be a local library
        # otherwise it is a third party library
        # subjected for checking and linking
        is_local_lib = os.path.exists(os.path.join(os.getcwd(), src_dir, library_full_str)) and \
            not match_excluded_dir_path(library, excluded_dir_paths)

        if not is_local_lib:
            library_env_key = LIB_KEY.upper() + '_' + library_full_str

            # if library key already added before, then ignore
            if not conf.env[library_env_key]:
                if project == GLOBAL_SETTINGS:
                    libpath_env_key = LIBPATH_KEY.upper()
                    conf.check_cxx(lib = library_full_str)
                else:
                    libpath_env_key = LIBPATH_KEY.upper() + '_' + project_use_key

                    conf.check_cxx(
                        use = project_use_key,
                        lib = library_full_str)

                # the string name after the underscore is the name of the usable stuff
                # such stuff can include INCLUDES, LIBPATH, LIB and some other settings
                for library_str in library_grp:
                    library_each_env_key = LIB_KEY.upper() + '_' + library_str

                    # store the library individually for build use
                    if not conf.env[library_each_env_key]:
                        conf.env[library_each_env_key] = library_str

        # add to be used libraries for project for build phase
        # regardless of whether it is project library or third-party library
        if project != GLOBAL_SETTINGS:
            # initialize the environment setting if not initalized yet
            if not conf.env[project_use_key]:
                conf.env[project_use_key] = []

            if is_local_lib:
                # append prefix (for local library) and suffix (for debug)
                # for local library, it is assumed that library is a single library string
                conf.env[project_use_key] += [ os.path.join(lib_dir, library + conf.env[TARGET_SUFFIX_KEY]) ]
            else:
                # for external library, the libraries will be individually appended
                for library_str in library_grp:
                    conf.env[project_use_key] += [ library_str ] 
 

def options(opt):
    # for switching between 32 and 64-bit builds
    opt.add_option('--m32', action = 'store_true', default = False, help = 'enable build in 32-bit')
    opt.add_option('--m64', action = 'store_true', default = False, help = 'enable build in 64-bit (default)')

    # for full static linking (recommended for alpine + musl only)
    opt.add_option('--static', action = 'store_true', default = False, help = 'enable full static linking')

    # for CXXFLAGS and LDFLAGS
    opt.add_option('--CXXFLAGS', action = 'store', default = '', help = 'additional global compiler flags to use')
    opt.add_option('--LDFLAGS', action = 'store', default = '', help = 'additional global linker flags to use')

    # retrieve the list of option values immediately to check if need to disable 64-bit build flag
    # disallow both m32 and m64 from being set at the same time
    (opts, arguments) = opt.parser.parse_args()

    # if both are false, then the configure process will check
    if opts.m32 == opts.m64 == True:
        opt.fatal('Only one of the options --m32 or --m64 can be applied')

    opt.load(COMPILER_CXX_KEY)


def configure_default_defines_flags(conf, define_value, cxxflags, ldflags):
    conf.define(define_value, 1)
    conf.env[CXXFLAGS_KEY.upper()] = cxxflags
    conf.env[LINKFLAGS_KEY.upper()] = ldflags


def configure_default_include_library_paths(conf, src_dir, var_dir, lib_dir):
    # appends default include and library paths
    default_include_paths = [ os.path.join(os.getcwd(), src_dir) ]
    default_library_paths = [ os.path.join(os.getcwd(), out, var_dir, lib_dir) ] 

    global_includes_env_key = INCLUDES_KEY.upper()
    global_libpath_env_key = LIBPATH_KEY.upper()

    if conf.env[global_includes_env_key]:
        conf.env[global_includes_env_key] += default_include_paths
    else:
        conf.env[global_includes_env_key] = default_include_paths

    if conf.env[global_libpath_env_key]:
        conf.env[global_libpath_env_key] += default_library_paths
    else:
        conf.env[global_libpath_env_key] = default_library_paths


def configure_global_and_project_options(conf):
    # prevents modification to the original project options
    import copy
    project_options_map_copy = copy.deepcopy(project_options_map)

    # for global settings first
    add_options_to_env(conf, GLOBAL_SETTINGS, global_options_map)

    for project in project_options_map_copy:
        add_options_to_env(conf, project, project_options_map_copy.get(project))


def configure(conf):
    # extract all the additional arguments from options
    if conf.options.m32 == conf.options.m64 == False:
        conf.options.m64 = True

    # set flag for full static linking
    is_static_link = conf.options.static

    # sets x86 or x64 flags depending on the platforms
    MSVC_TARGETS_KEY = 'MSVC_TARGETS'
    UNIX_X86_FLAG = '-m32'
    UNIX_X64_FLAG = '-m64'
    WINDOWS_X86_FLAG = 'x86'
    WINDOWS_X64_FLAG = 'x64'

    if IS_PLATFORM_LINUX or IS_PLATFORM_CYGWIN or IS_PLATFORM_OTHERS:
        if conf.options.m32:
            common_cxxflags.append(UNIX_X86_FLAG)
            common_ldflags.append(UNIX_X86_FLAG)
        else:
            common_cxxflags.append(UNIX_X64_FLAG)
            common_ldflags.append(UNIX_X64_FLAG)

    # combine common flags
    combined_cxxflags = [ conf.options.CXXFLAGS ] + common_cxxflags
    combined_ldflags = [ conf.options.LDFLAGS ] + common_ldflags

    if is_static_link:
        combined_ldflags.append('-static')

    # for debug build configuration settings
    conf.setenv(BLD_CONF_DEBUG)

    if IS_PLATFORM_WINDOWS:
        if conf.options.m32:
            conf.env[MSVC_TARGETS_KEY] = [WINDOWS_X86_FLAG]
        else:
            conf.env[MSVC_TARGETS_KEY] = [WINDOWS_X64_FLAG]

    conf.env[BLD_CONF_KEY] = BLD_CONF_DEBUG
    conf.env[TARGET_SUFFIX_KEY] = DEBUG_SUFFIX
    conf.env[STATIC_FLAG] = is_static_link
    conf.load(COMPILER_CXX_KEY)

    if is_static_link:
        conf.env[SHLIB_MARKER] = ''

    configure_default_defines_flags(conf, DEBUG_DEFINE_VALUE,
        combined_cxxflags + debug_cxxflags,
        combined_ldflags + debug_ldflags)

    configure_default_include_library_paths(conf, src_dir, dbg_dir, lib_dir)
    configure_global_and_project_options(conf)

    # for release build configuration settings
    # release is the default build
    conf.setenv(BLD_CONF_RELEASE)

    if IS_PLATFORM_WINDOWS:
        if conf.options.m32:
            conf.env[MSVC_TARGETS_KEY] = [WINDOWS_X86_FLAG]
        else:
            conf.env[MSVC_TARGETS_KEY] = [WINDOWS_X64_FLAG]

    conf.env[BLD_CONF_KEY] = BLD_CONF_RELEASE
    conf.env[TARGET_SUFFIX_KEY] = RELEASE_SUFFIX
    conf.env[STATIC_FLAG] = is_static_link
    conf.load(COMPILER_CXX_KEY)

    if is_static_link:
        conf.env[SHLIB_MARKER] = ''

    configure_default_defines_flags(conf, RELEASE_DEFINE_VALUE,
        combined_cxxflags + release_cxxflags,
        combined_ldflags + release_ldflags)

    configure_default_include_library_paths(conf, src_dir, rel_dir, lib_dir)
    configure_global_and_project_options(conf)


def is_binary_project(project_path, src_dir, bin_project_paths):
    for bin_project_path in bin_project_paths:
        if project_path.abspath() == os.path.join(os.getcwd(), src_dir, bin_project_path):
            return True

    return False


def build_helper(bld, src_sub_dir_path, project_options_map):
    # check for invalid source dir and matching excluded directory paths
    if not src_sub_dir_path or \
        match_excluded_dir_path(str(remove_first_dir_from_path(src_sub_dir_path.srcpath())), excluded_dir_paths):
        return

    # get the file or directory name in current directory
    for src_child_name in src_sub_dir_path.listdir():
        src_child_path = src_sub_dir_path.find_dir(src_child_name)

        # if path is a directory
        if src_child_path:
            build_helper(bld, src_child_path, project_options_map)

    # searches for all source files in the current sub-directory
    global src_exts

    if not isinstance(src_exts, list):
        src_exts = src_exts.split()

    src_files = src_sub_dir_path.ant_glob(['*{0}'.format(src_ext) for src_ext in src_exts])

    # if there is at least one source file, perform compilation for this directory
    if src_files:
        project = remove_first_dir_from_path(src_sub_dir_path.srcpath())
        proj_use_key = project + '_' + USE_ENV_KEY
        linking_libraries = [ proj_use_key ]
        target_suffix = bld.env[TARGET_SUFFIX_KEY]

        if bld.env[proj_use_key]:
            linking_libraries += bld.env[proj_use_key]

        if is_binary_project(src_sub_dir_path, src_dir, bin_project_paths):
            bin_target_path = src_sub_dir_path.srcpath().replace(src_dir, bin_dir) + target_suffix

            # builds the executable with rpath pointing to the residing directory
            bld.program(
                source = src_files,
                target = bin_target_path,
                use = linking_libraries,
                rpath = RPATHS_PROGRAM,
                install_path = INSTALL_BIN_PATH
            )
        else:
            shared_lib_path = src_sub_dir_path.srcpath().replace(src_dir, lib_dir)
            shared_lib_target_path = shared_lib_path + target_suffix
            
            static_lib_path = shared_lib_path + static_lib_suffix
            static_lib_target_path = static_lib_path + target_suffix

            # builds the static library
            bld.stlib(
                source = src_files,
                target = static_lib_target_path,
                install_path = INSTALL_LIB_PATH
            )

            # builds the shared library with rpath pointing to residing directory
            if not bld.env[STATIC_FLAG]:
                bld.shlib(
                    source = src_files,
                    target = shared_lib_target_path,
                    use = linking_libraries,
                    rpath = RPATHS_PROGRAM,
                    install_path = INSTALL_LIB_PATH
                )


def build(bld):
    # prevents modification to the project options
    import copy
    project_options_map_copy = copy.deepcopy(project_options_map)
    build_helper(bld, bld.path.find_dir(src_dir), project_options_map_copy)


from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext, ListContext, StepContext

# enables release and debug keywords to be used for simplicity
class release(BuildContext):
    'builds in release mode, same as waf build without arguments command'
    cmd = BLD_CONF_RELEASE
    variant = rel_dir

class debug(BuildContext):
    'builds in debug mode'
    cmd = BLD_CONF_DEBUG
    variant = dbg_dir

# enhance the default 'build' command (which means 'waf' or 'waf build' was used)
# variant is set to default, which is BLD_CONF_RELEASE
class build_default(BuildContext):
    cmd = BUILD_COMMAND
    variant = default_bld_dir

# set the same build configuration defaults to the other contexts
class clean_default(CleanContext):
    cmd = CLEAN_COMMAND
    variant = default_bld_dir

class install_default(InstallContext):
    cmd = INSTALL_COMMAND
    variant = default_bld_dir

class uninstall_default(UninstallContext):
    cmd = UNINSTALL_COMMAND
    variant = default_bld_dir

# other commands to set the variant
class list_default(ListContext):
    cmd = LIST_COMMAND
    variant = default_bld_dir

class step_default(StepContext):
    cmd = STEP_COMMAND
    variant = default_bld_dir

# sets debug and release build configurations for all the different build, clean, install and uninstall contexts
def init(ctx):
    for bld_mode in [ BLD_CONF_RELEASE, BLD_CONF_DEBUG ]:
        for context in (BuildContext, CleanContext, InstallContext, UninstallContext, ListContext, StepContext):
            name = context.__name__.replace('Context', '').lower()

            class Composite(context):
                cmd = name + '-' + bld_mode
                variant = bld_mode

