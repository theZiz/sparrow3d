#define __SPARROW_INTERNAL_ZBOTH__
#ifndef __SPARROW_INTERNAL_PERSPECT__
	#ifdef __SPARROW_INTERNAL_ALPHA__
		#include "sparrowPrimitiveTriangleInclude.c"
	#endif
#endif
#include "sparrowPrimitiveTexTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZBOTH__
#define __SPARROW_INTERNAL_ZTEST__
#ifndef __SPARROW_INTERNAL_PERSPECT__
	#ifdef __SPARROW_INTERNAL_ALPHA__
		#include "sparrowPrimitiveTriangleInclude.c"
	#endif
#endif
#include "sparrowPrimitiveTexTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZTEST__
#define __SPARROW_INTERNAL_ZSET__
#ifndef __SPARROW_INTERNAL_PERSPECT__
	#ifdef __SPARROW_INTERNAL_ALPHA__
		#include "sparrowPrimitiveTriangleInclude.c"
	#endif
#endif
#include "sparrowPrimitiveTexTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZSET__
#define __SPARROW_INTERNAL_ZNOTHING__
#ifndef __SPARROW_INTERNAL_PERSPECT__
	#ifdef __SPARROW_INTERNAL_ALPHA__
		#include "sparrowPrimitiveTriangleInclude.c"
	#endif
#endif
#include "sparrowPrimitiveTexTriangleInclude.c"
#undef __SPARROW_INTERNAL_ZNOTHING__
