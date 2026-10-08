# Compiler and linker options for our own targets (warnings, sanitizers,
# runtime linking). Never applied to third-party code.

function(csopesy_set_warnings target)
  if(MSVC)
    set(warnings /W4 /permissive-)
    if(CSOPESY_WARNINGS_AS_ERRORS)
      list(APPEND warnings /WX)
    endif()
  else()
    set(warnings
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wold-style-cast
        -Wnon-virtual-dtor)
    if(CSOPESY_WARNINGS_AS_ERRORS)
      list(APPEND warnings -Werror)
    endif()
  endif()
  target_compile_options(${target} PRIVATE ${warnings})
endfunction()

# MinGW GCC ships no ASan/UBSan runtime, and MSVC/clang-cl use a different
# sanitizer model, so sanitizers are only enabled outside Windows.
function(csopesy_enable_sanitizers target)
  if(NOT CSOPESY_ENABLE_SANITIZERS OR WIN32)
    return()
  endif()
  set(sanitizers -fsanitize=address,undefined -fno-omit-frame-pointer)
  target_compile_options(${target} PRIVATE ${sanitizers})
  target_link_options(${target} PRIVATE ${sanitizers})
endfunction()

# Lets an exe run without MinGW on PATH, and stops it from loading a
# mismatched runtime from another MinGW on PATH (such as Git's): links the C++
# runtime statically and copies libwinpthread next to the exe. A fully static
# link fails with the MinGW toolchain bundled with CLion.
function(csopesy_use_portable_runtime target)
  if(NOT MINGW)
    return()
  endif()
  target_link_options(${target} PRIVATE -static-libgcc -static-libstdc++)
  get_filename_component(mingw_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
  if(EXISTS "${mingw_bin}/libwinpthread-1.dll")
    add_custom_command(
      TARGET ${target}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${mingw_bin}/libwinpthread-1.dll"
              "$<TARGET_FILE_DIR:${target}>"
      VERBATIM)
  endif()
endfunction()
