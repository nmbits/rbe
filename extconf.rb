require 'mkmf'

if find_header("BeBuild.h", "/boot/system/develop/headers/os") &&
   have_library('be')
  with_cflags "-g -DDEBUG" do true end
  with_cppflags "-g -std=c++11 -DDEBUG" do true end
  create_makefile 'rbe', 'build_src'
end
