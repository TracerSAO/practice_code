file(GLOB ROOT_SOURCE CONFIGURE_DEPENDS *.cpp)
file(GLOB ROOT_INLINE CONFIGURE_DEPENDS *.inl)
file(GLOB ROOT_HEADER CONFIGURE_DEPENDS *.hpp)
file(GLOB ROOT_TEXT   CONFIGURE_DEPENDS ../*.txt)
file(GLOB ROOT_MD     CONFIGURE_DEPENDS ../*.md)
file(GLOB ROOT_NAT    CONFIGURE_DEPENDS ../util/glm.natvis)

file(GLOB_RECURSE CORE_SOURCE CONFIGURE_DEPENDS ./detail/*.cpp)
file(GLOB_RECURSE CORE_INLINE CONFIGURE_DEPENDS ./detail/*.inl)
file(GLOB_RECURSE CORE_HEADER CONFIGURE_DEPENDS ./detail/*.hpp)

file(GLOB_RECURSE EXT_SOURCE CONFIGURE_DEPENDS ./ext/*.cpp)
file(GLOB_RECURSE EXT_INLINE CONFIGURE_DEPENDS ./ext/*.inl)
file(GLOB_RECURSE EXT_HEADER CONFIGURE_DEPENDS ./ext/*.hpp)

file(GLOB_RECURSE GTC_SOURCE CONFIGURE_DEPENDS ./gtc/*.cpp)
file(GLOB_RECURSE GTC_INLINE CONFIGURE_DEPENDS ./gtc/*.inl)
file(GLOB_RECURSE GTC_HEADER CONFIGURE_DEPENDS ./gtc/*.hpp)

file(GLOB_RECURSE GTX_SOURCE CONFIGURE_DEPENDS ./gtx/*.cpp)
file(GLOB_RECURSE GTX_INLINE CONFIGURE_DEPENDS ./gtx/*.inl)
file(GLOB_RECURSE GTX_HEADER CONFIGURE_DEPENDS ./gtx/*.hpp)

file(GLOB_RECURSE SIMD_SOURCE CONFIGURE_DEPENDS ./simd/*.cpp)
file(GLOB_RECURSE SIMD_INLINE CONFIGURE_DEPENDS ./simd/*.inl)
file(GLOB_RECURSE SIMD_HEADER CONFIGURE_DEPENDS ./simd/*.h)

source_group("Text Files" FILES ${ROOT_TEXT} ${ROOT_MD})
source_group("Core Files" FILES ${CORE_SOURCE})
source_group("Core Files" FILES ${CORE_INLINE})
source_group("Core Files" FILES ${CORE_HEADER})
source_group("EXT Files" FILES ${EXT_SOURCE})
source_group("EXT Files" FILES ${EXT_INLINE})
source_group("EXT Files" FILES ${EXT_HEADER})
source_group("GTC Files" FILES ${GTC_SOURCE})
source_group("GTC Files" FILES ${GTC_INLINE})
source_group("GTC Files" FILES ${GTC_HEADER})
source_group("GTX Files" FILES ${GTX_SOURCE})
source_group("GTX Files" FILES ${GTX_INLINE})
source_group("GTX Files" FILES ${GTX_HEADER})
source_group("SIMD Files" FILES ${SIMD_SOURCE})
source_group("SIMD Files" FILES ${SIMD_INLINE})
source_group("SIMD Files" FILES ${SIMD_HEADER})

add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE ../)

if(BUILD_STATIC_LIBS)
add_library(glm_static STATIC ${ROOT_TEXT} ${ROOT_MD} ${ROOT_NAT}
	${ROOT_SOURCE}    ${ROOT_INLINE}    ${ROOT_HEADER}
	${CORE_SOURCE}    ${CORE_INLINE}    ${CORE_HEADER}
	${EXT_SOURCE}     ${EXT_INLINE}     ${EXT_HEADER}
	${GTC_SOURCE}     ${GTC_INLINE}     ${GTC_HEADER}
	${GTX_SOURCE}     ${GTX_INLINE}     ${GTX_HEADER}
	${SIMD_SOURCE}    ${SIMD_INLINE}    ${SIMD_HEADER})
	target_link_libraries(glm_static PUBLIC glm)
	add_library(glm::glm_static ALIAS glm_static)
endif()

if(BUILD_SHARED_LIBS)
add_library(glm_shared SHARED ${ROOT_TEXT} ${ROOT_MD} ${ROOT_NAT}
	${ROOT_SOURCE}    ${ROOT_INLINE}    ${ROOT_HEADER}
	${CORE_SOURCE}    ${CORE_INLINE}    ${CORE_HEADER}
	${EXT_SOURCE}     ${EXT_INLINE}     ${EXT_HEADER}
	${GTC_SOURCE}     ${GTC_INLINE}     ${GTC_HEADER}
	${GTX_SOURCE}     ${GTX_INLINE}     ${GTX_HEADER}
	${SIMD_SOURCE}    ${SIMD_INLINE}    ${SIMD_HEADER})
	target_link_libraries(glm_shared PUBLIC glm)
	add_library(glm::glm_shared ALIAS glm_shared)
endif()
