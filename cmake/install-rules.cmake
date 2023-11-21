install(
    TARGETS BasicSnake_exe
    RUNTIME COMPONENT BasicSnake_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
