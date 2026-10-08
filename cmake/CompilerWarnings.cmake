# Warning and sanitizer flags. Applied to our own targets only, never to
# third-party code.

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
