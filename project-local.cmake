################################################################################
# Configuration Options                                                        #
################################################################################
set(WITH_FOOTBOT_LEDS "NO" CACHE STRING "Enable footbot robots to control their LEDS via actuators")
set(WITH_FOOTBOT_RAB "NO" CACHE STRING "Enable footbot robots to read/write over the RAB medium via sensors/actuators.")
set(WITH_FOOTBOT_BATTERY "NO" CACHE STRING "Enable footbot robots to use the battery.")
define_property(CACHED_VARIABLE PROPERTY "WITH_FOOTBOT_LEDS"
  BRIEF_DOCS "Enable footbot robots to control their LEDS. Only for simulated robots."
  FULL_DOCS "Default=NO.")
define_property(CACHED_VARIABLE PROPERTY "WITH_FOOTBOT_RAB"
  BRIEF_DOCS "Enable footbot robots to use the RAB medium. Only for simulated robots"
  FULL_DOCS "Default=NO.")
define_property(CACHED_VARIABLE PROPERTY "WITH_FOOTBOT_BATTERY"
  BRIEF_DOCS "Enable footbot robots to use the battery. Only for simulation robots."
  FULL_DOCS "Default=NO.")

set(LIBRA_BUILD_FOR "ARGOS" CACHE STRING "Build for ARGoS.")

################################################################################
# External Projects                                                            #
################################################################################
set(${target}_CHECK_LANGUAGE "CXX")

if("${LIBRA_BUILD_FOR}" MATCHES "MSI" )
  message(STATUS "Building for MSI")
  set(LOCAL_INSTALL_PREFIX /home/gini/shared/swarm/$ENV{MSICLUSTER})
elseif("${LIBRA_BUILD_FOR}" MATCHES "ARGOS")
  message(STATUS "Building for ARGoS")
  set(LOCAL_INSTALL_PREFIX /opt/data/local)
elseif("${LIBRA_BUILD_FOR}" MATCHES "EV3")
  message(STATUS "Building for EV3")
else()
  message(FATAL_ERROR
    "Unknown build target '${LIBRA_BUILD_FOR}'. Must be: [MSI,ARGOS,EV3]")
endif()

# Support libraries
add_subdirectory(ext/cosm)

set(FORDYCA_WITH_VIS "${COSM_WITH_VIS}")

if (${FORDYCA_WITH_VIS})
  set(CMAKE_AUTOMOC ON)
  find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
  set(CMAKE_AUTOMOC OFF)
endif()

################################################################################
# Sources                                                                      #
################################################################################
if (NOT ${FORDYCA_WITH_VIS})
    list(REMOVE_ITEM ${target}_ROOT_SRC
    ${${target}_SRC_PATH}/support/los_visualizer.cpp
    ${${target}_SRC_PATH}/support/depth0/depth0_qt_user_functions.cpp
    ${${target}_SRC_PATH}/support/depth1/depth1_qt_user_functions.cpp
    ${${target}_SRC_PATH}/support/depth2/depth2_qt_user_functions.cpp)
endif()

################################################################################
# Libraries                                                                    #
################################################################################
# Define link libraries
set(${target}_LIBRARIES
  cosm
  ${cosm_LIBRARIES}
  nlopt
  stdc++fs
  rt)

# Define link search dirs
set(${target}_LIBRARY_DIRS
  ${rcppsw_LIBRARY_DIRS}
  ${cosm_LIBRARY_DIRS})

if ("${LIBRA_BUILD_FOR}" MATCHES "ARGOS" OR "${LIBRA_BUILD_FOR}" MATCHES "MSI")
  set(argos3_LIBRARIES
  argos3core_simulator
  argos3plugin_simulator_footbot
  argos3plugin_simulator_entities
  argos3plugin_simulator_dynamics2d
  argos3plugin_simulator_genericrobot
  argos3plugin_simulator_qtopengl
  argos3plugin_simulator_media
  )

  set (${target}_LIBRARIES
    ${${target}_LIBRARIES}
    ${argos3_LIBRARIES}
    )

  set(${target}_LIBRARY_DIRS
    ${${target}_LIBRARY_DIRS}
    /usr/lib/argos3
    /usr/local/lib/argos3
    ${LOCAL_INSTALL_PREFIX}/lib/argos3
    )
  if ("${LIBRA_BUILD_FOR}" MATCHES "MSI")
    # For nlopt
    set(${target}_LIBRARY_DIRS
      ${$target}_LIBRARY_DIRS}
      ${LOCAL_INSTALL_PREFIX}/lib/argos3
      ${LOCAL_INSTALL_PREFIX}/lib64)
  endif()
endif()

# Force failures at build time rather than runtime
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

link_directories(${${target}_LIBRARY_DIRS})
add_library(${target} SHARED ${${target}_ROOT_SRC})
add_dependencies(${target} rcppsw cosm)

################################################################################
# Includes                                                                     #
################################################################################
set(${target}_INCLUDE_DIRS
  "${${target}_INC_PATH}"
  ${rcppsw_INCLUDE_DIRS}
  ${cosm_INCLUDE_DIRS}
  /usr/include/eigen3)

set(${target}_SYS_INCLUDE_DIRS
  ${cosm_SYS_INCLUDE_DIRS}
  ${NLOPT_INCLUDE_DIRS})

if ("${LIBRA_BUILD_FOR}" MATCHES "ARGOS")
  set(${target}_SYS_INCLUDE_DIRS
    ${${target}_SYS_INCLUDE_DIRS}
  ${LOCAL_INSTALL_PREFIX}/include
  )
endif()

target_include_directories(${target} PUBLIC ${${target}_INCLUDE_DIRS})

target_include_directories(${target} SYSTEM PUBLIC
  /usr/include/lua5.3 # Not needed for compiling, but for emacs rtags
  /usr/local/include
  ${nlopt_INCLUDE_DIRS}
  ${${target}_SYS_INCLUDE_DIRS}
  )

target_link_libraries(${target} ${${target}_LIBRARIES} cosm nlopt)

################################################################################
# Compile Options/Definitions                                                  #
################################################################################
if ("${LIBRA_BUILD_FOR}" MATCHES "ARGOS")
  if (WITH_FOOTBOT_LEDS)
    target_compile_definitions(${target} PUBLIC FORDYCA_WITH_ROBOT_LEDS)
  endif()
  if (WITH_FOOTBOT_RAB)
    target_compile_definitions(${target} PUBLIC FORDYCA_WITH_ROBOT_RAB)
  endif()
  if (WITH_FOOTBOT_BATTERY)
    target_compile_definitions(${target} PUBLIC FORDYCA_WITH_ROBOT_BATTERY)
  endif()

endif()


if ("${LIBRA_BUILD_FOR}" MATCHES "MSI")
  target_compile_options(${target} PUBLIC
    -Wno-missing-include-dirs
    -fno-new-inheriting-ctors)
endif()

################################################################################
# Exports                                                                      #
################################################################################
if (NOT IS_ROOT_PROJECT)
  set(${target}_INCLUDE_DIRS ${${target}_INCLUDE_DIRS} PARENT_SCOPE)
  set(${target}_LIBRARY_DIRS ${${target}_LIBRARY_DIRS} PARENT_SCOPE)
  set(${target}_LIBRARIES ${${target}_LIBRARIES} PARENT_SCOPE)
endif()
