
#include <stdio.h>
#include <netcdf.h>
#include <string.h>

#include <Rdefines.h>

/* These same values are hard-coded into the R source. Don't change them! */
#define R_NC_TYPE_SHORT 1
#define R_NC_TYPE_INT   2
#define R_NC_TYPE_FLOAT 3
#define R_NC_TYPE_DOUBLE 4
#define R_NC_TYPE_TEXT  5

/*********************************************************************/
/* Returns a vector of dim sizes for the variable */
void R_nc_varsize( int *ncid, int *varid, int *varsize, int *retval )
{
	int 	i, err, ndims, dimid[NC_MAX_DIMS];
	size_t	dimlen;

	*retval = 0;

	/* Get ndims */
	err = nc_inq_varndims( *ncid, *varid, &ndims );
	if( err != NC_NOERR ) {
		fprintf( stderr, "Error in R_nc_varsize on nc_inq_varndims call: %s\n", 
			nc_strerror(err) );
		*retval = -1;
		return;
		}

	/* Get dimids */
	err = nc_inq_vardimid( *ncid, *varid, dimid );
	if( err != NC_NOERR ) {
		fprintf( stderr, "Error in R_nc_varsize on nc_inq_vardimid call: %s\n",
			nc_strerror(err) );
		*retval = -1;
		return;
		}

	/* Get size of each dim */
	for( i=0; i<ndims; i++ ) {
		err = nc_inq_dimlen( *ncid, dimid[i], &dimlen );
		if( err != NC_NOERR ) {
			fprintf( stderr, "Error in R_nc_varsize on nc_inq_dimlen call: %s\n",
				nc_strerror(err) );
			*retval = -1;
			return;
			}
		varsize[i] = (int)dimlen;
		}
}

/*********************************************************************/
/* Returns 1 if this var has an unlimited dimension, 0 otherwise */
void R_nc_inq_varunlim( int *ncid, int *varid, int *isunlim, int *retval )
{
	int	i, ndims, unlimdimid, dimids[MAX_NC_DIMS], nvars, err;

	/* Get the unlimited dim id */
	*retval = nc_inq_unlimdim( *ncid, &unlimdimid );
	if( *retval != NC_NOERR ) {
		fprintf( stderr, "Error in R_nc_inq_varunlim while getting unlimdimid: %s\n", 
			nc_strerror(*retval) );
		return;
		}

	/* Get this var's ndims */
	*retval = nc_inq_varndims( *ncid, *varid, &ndims );
	if( *retval != NC_NOERR ) {
		fprintf( stderr, "Error in R_nc_inq_varunlim while getting ndims: %s\n", 
			nc_strerror(*retval) );
		fprintf( stderr, "Using ncid=%d and varid=%d\n", 
			*ncid, *varid );
		return;
		}

	/* Get this var's dims */
	*retval = nc_inq_vardimid( *ncid, *varid, dimids );
	if( *retval != NC_NOERR ) {
		fprintf( stderr, "Error in R_nc_inq_varunlim while getting dimids: %s\n", 
			nc_strerror(*retval) );
		return;
		}

	*isunlim = 0;
	for( i=0; i<ndims; i++ )
		if( dimids[i] == unlimdimid ) {
			*isunlim = 1;
			break;
			}
}

/*********************************************************************/
/* Note that space for all returned values MUST already be declared! */
void R_nc_inq_var( int *ncid, int *varid, char **varname, 
	int *type, int *ndims, int *dimids, int *natts, int *precint, int *retval )
{
	nc_type nct;

	*retval = nc_inq_var(*ncid, *varid, varname[0], &nct,  
		ndims, dimids, natts );
	*type = (int)nct;

	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_inq_var: %s\n", 
			nc_strerror(*retval) );

	if( nct == NC_CHAR )
		*precint = R_NC_TYPE_TEXT;
	else if( nct == NC_SHORT )
		*precint = R_NC_TYPE_SHORT;
	else if( nct == NC_INT )
		*precint = R_NC_TYPE_INT;
	else if( nct == NC_FLOAT )
		*precint = R_NC_TYPE_FLOAT;
	else if( nct == NC_DOUBLE )
		*precint = R_NC_TYPE_DOUBLE;
	else
		*precint = -1;
}

/*********************************************************************/
/* Note that space for returned value MUST already be declared! */
void R_nc_inq_varname( int *ncid, int *varid, char **varname, int *retval )
{
	*retval = nc_inq_varname(*ncid, *varid, varname[0] );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_inq_varname: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_inq_varndims( int *ncid, int *varid, int *ndims, int *retval )
{
	*retval = nc_inq_varndims(*ncid, *varid, ndims );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_inq_varndims: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_get_vara_double( int *ncid, int *varid, int *start, 
	int *count, double *data, int *retval )
{
	int	i, err, ndims;
	size_t	s_start[MAX_NC_DIMS], s_count[MAX_NC_DIMS];
	char	vn[2048];

	err = nc_inq_varndims(*ncid, *varid, &ndims );
	if( err != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_get_vara_double while getting ndims: %s\n", 
			nc_strerror(*retval) );

	for( i=0; i<ndims; i++ ) {
		s_start[i] = (size_t)start[i];
		s_count[i] = (size_t)count[i];
		}
		
	*retval = nc_get_vara_double(*ncid, *varid, s_start, s_count, data );
	if( *retval != NC_NOERR ) {
		nc_inq_varname( *ncid, *varid, vn );
		fprintf( stderr, "Error in R_nc_get_vara_double: %s\n", 
			nc_strerror(*retval) );
		fprintf( stderr, "Var: %s  Ndims: %d   Start: ", vn, ndims );
		for( i=0; i<ndims; i++ ) {
			fprintf( stderr, "%ld", s_start[i] );
			if( i < ndims-1 )
				fprintf( stderr, "," );
			}
		fprintf( stderr, "Count: " );
		for( i=0; i<ndims; i++ ) {
			fprintf( stderr, "%ld", s_count[i] );
			if( i < ndims-1 )
				fprintf( stderr, "," );
			}
		}
}

/*********************************************************************/
void R_nc_get_var_double( int *ncid, int *varid, double *data, int *retval )
{
	*retval = nc_get_var_double(*ncid, *varid, data );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_get_var_double: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
/* Returns -1 if the dim is not found in the file */
void R_nc_inq_dimid( int *ncid, char **dimname, int *dimid )
{
	int err;
	err = nc_inq_dimid(*ncid, dimname[0], dimid );
	if( err != NC_NOERR ) 
		*dimid = -1;
}

/*********************************************************************/
/* Returns -1 if the var is not found in the file */
void R_nc_inq_varid( int *ncid, char **varname, int *varid )
{
	int err;
	err = nc_inq_varid(*ncid, varname[0], varid );
	if( err != NC_NOERR ) 
		*varid = -1;
}

/*********************************************************************/
void R_nc_inq_unlimdim( int *ncid, int *unlimdimid, int *retval )
{
	*retval = nc_inq_unlimdim(*ncid, unlimdimid );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_inq_unlimdim: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_inq_dim( int *ncid, int *dimid, char **dimname, int *dimlen,
				int *retval )
{
	char name[NC_MAX_NAME];
	size_t len;

	*retval = nc_inq_dim(*ncid, *dimid, name, &len);
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_inq_dim: %s\n", 
			nc_strerror(*retval) );
	*dimlen = (int)len;
	/* NOTE NOTE NOTE!! This assumes that the calling process
	 * allocated storage of at least NC_MAX_NAME!
	 */
	strcpy(dimname[0], name);
}

/*********************************************************************/
void R_nc_inq( int *ncid, int *ndims, int *nvars, int *natts,
	int *unlimdimid, int *retval )
{
	*retval = nc_inq(*ncid, ndims, nvars, natts, unlimdimid );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_inq: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
/* cmode is 0 for read only, 1 for write access.
 */
void R_nc_open( char **filename, int *cmode, int *ncid, int *retval )
{
	int	nc_mode;

	if( *cmode == 0 )
		nc_mode = 0;
	else if( *cmode == 1 )
		nc_mode = NC_WRITE;
	else
		{
		fprintf( stderr, "Error in R_nc_open: bad mode passed.  Must be 0 (read) or 1 (write)\n");
		*retval = -1;
		return;
		}

	*retval = nc_open(R_ExpandFileName(filename[0]), nc_mode, ncid);
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_open: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_create( char **filename, int *cmode, int *ncid, int *retval )
{
	*retval = nc_create(R_ExpandFileName(filename[0]), *cmode, ncid);
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_create: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
nc_type R_nc_ttc_to_nctype( int type_to_create )
{
	/* These values are from the R code, they must match exactly */
	if( type_to_create == 1 )
		return( NC_SHORT );
	if( type_to_create == 2 )
		return( NC_INT );
	if( type_to_create == 3 )
		return( NC_FLOAT );
	if( type_to_create == 4 )
		return( NC_DOUBLE );
	if( type_to_create == 5 )
		return( NC_CHAR );

	fprintf(stderr,"Error, R_nc_ttc_to_nctype passed unknown value: %d\n",
		type_to_create );
	exit(-1);
}

/*********************************************************************/
void R_nc_put_att_int( int *ncid, int *varid, char **attname, 
		int *type_to_create, int *natts, int *attribute, int *retval )
{
	nc_type ttc;
	ttc = R_nc_ttc_to_nctype( *type_to_create );
	*retval = nc_put_att_int(*ncid, *varid, attname[0], 
		ttc, *natts, attribute );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_put_att_double: %s\n", 
			nc_strerror(*retval) );
}

/* NOTE: if you think a R_nc_put_att_float function should be here, it means
 * you are confused.  The type attribute on the end of these function calls
 * (such as R_nc_put_att_XXX) means "XXX" is the R storage mode, NOT the
 * type of the attribute!  R storage modes can only be integer, text, or
 * double (for our purposes); there is no "float" storage mode in R. Hence,
 * there is no R_nc_put_att_float function.  The type of attribute to be
 * created can, of course, be float, and that is indicated by type_to_create==3.
 */

/*********************************************************************/
void R_nc_put_att_double( int *ncid, int *varid, char **attname, 
		int *type_to_create, int *natts, double *attribute, int *retval )
{
	nc_type ttc;
	ttc = R_nc_ttc_to_nctype( *type_to_create );
	*retval = nc_put_att_double(*ncid, *varid, attname[0], 
		ttc, *natts, attribute );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_put_att_double: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_put_att_text( int *ncid, int *varid, char **attname, 
		int *type_to_create, int *natts, char **attribute, int *retval )
{
	nc_type ttc;
	size_t attlen;

	ttc = R_nc_ttc_to_nctype( *type_to_create );
	/* For some reason the C interface does not include the nc_type for this call */

	attlen = strlen(attribute[0]);
	*retval = nc_put_att_text(*ncid, *varid, attname[0], 
		attlen, attribute[0] );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_put_att_text: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
/* NOTE that space for the attribute must already be allocated! */
void R_nc_get_att_int( int *ncid, int *varid, char **attname, 
			int *attribute, int *retval )
{
	int	err;
	size_t	attlen;

	*retval = nc_get_att_int(*ncid, *varid, attname[0], attribute);
}

/*********************************************************************/
/* NOTE that space for the attribute must already be allocated! */
void R_nc_get_att_double( int *ncid, int *varid, char **attname, 
			double *attribute, int *retval )
{
	int	err;
	size_t	attlen;

	*retval = nc_get_att_double(*ncid, *varid, attname[0], attribute);
}

/*********************************************************************/
/* If returned value 'retval' is 0, then returned value 'type' will hold
 * the type of the named attribute, and returned value 'attlen' will
 * hold the length of the named attribute.  If returned value 'retval'
 * is NOT 0, then the specified variable did not have an attribute
 * named 'attname'.
 */
void R_nc_inq_att( int *ncid, int *varid, char **attname, int *type,
			int *attlen, int *retval )
{
	size_t  s_attlen;
	nc_type nctype;

	*retval = nc_inq_att(*ncid, *varid, attname[0], &nctype, &s_attlen );
	if( (*retval != 0) && (*retval != NC_ENOTATT)) 
		fprintf( stderr, "Error in R_nc_inq_att: while looking for attribute %s, got error %s\n",
			attname[0], nc_strerror(*retval) );

	if( *retval == 0 ) {
		if( nctype == NC_CHAR )
			*type = R_NC_TYPE_TEXT;
		else if( nctype == NC_SHORT )
			*type = R_NC_TYPE_SHORT;
		else if( nctype == NC_INT )
			*type = R_NC_TYPE_INT;
		else if( nctype == NC_FLOAT )
			*type = R_NC_TYPE_FLOAT;
		else if( nctype == NC_DOUBLE )
			*type = R_NC_TYPE_DOUBLE;
		else if( nctype == NC_NAT )
			fprintf( stderr, "Error in R_nc_inq_att: not set up to handle attributes of type \"NAT\"!  Netcdf type code: %d Attribute name: %s\n", nctype, attname[0] );
		else if( nctype == NC_BYTE )
			fprintf( stderr, "Error in R_nc_inq_att: not set up to handle attributes of type \"BYTE\"!  Netcdf type code: %d Attribute name: %s\n", nctype, attname[0] );
		else
			{
			fprintf( stderr, "Error in R_nc_inq_att: not set up to handle attributes of this type!  Netcdf type code: %d Attribute name: %s\n", nctype, attname[0] );
			*retval = -1;
			}

		*attlen = (int)s_attlen;
		}
}

/*********************************************************************/
/* NOTE that space for the attribute must already be allocated! */
void R_nc_get_att_text( int *ncid, int *varid, char **attname, 
			char **attribute, int *retval )
{
	int	err;
	size_t	attlen;

	*retval = nc_get_att_text(*ncid, *varid, attname[0], 
		attribute[0] );
	/* append a NULL */
	if( *retval != NC_NOERR ) {
		strcpy( attribute[0], "\0" );
		return;
		}
	err = nc_inq_attlen( *ncid, *varid, attname[0], &attlen );
	if( err != NC_NOERR ) {
		strcpy( attribute[0], "\0" );
		return;
		}
	attribute[0][attlen] = '\0';
}

/*********************************************************************/
void R_nc_put_vara_double( int *ncid, int *varid, int *start,
	int *count, double *data, int *retval )
{
	int i, ndims, err;
	size_t s_start[MAX_NC_DIMS], s_count[MAX_NC_DIMS];

	/* Get # of dims for this var */
	err = nc_inq_ndims( *ncid, &ndims );
	if( err != NC_NOERR )
		fprintf( stderr, "Error on nc_inq_ndims call in R_nc_put_vara_double: %s\n", 
			nc_strerror(*retval) );

	/* Copy over from ints to size_t */
	for( i=0; i<ndims; i++ ) {
		s_start[i] = (size_t)start[i];
		s_count[i] = (size_t)count[i];
		} 
	*retval = nc_put_vara_double(*ncid, *varid, s_start, s_count, data );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_put_vara_double: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_put_vara_int( int *ncid, int *varid, int *start,
	int *count, int *data, int *retval )
{
	int i, ndims, err;
	size_t s_start[MAX_NC_DIMS], s_count[MAX_NC_DIMS];

	/* Get # of dims for this var */
	err = nc_inq_ndims( *ncid, &ndims );
	if( err != NC_NOERR )
		fprintf( stderr, "Error on nc_inq_ndims call in R_nc_put_vara_int: %s\n", 
			nc_strerror(*retval) );

	/* Copy over from ints to size_t */
	for( i=0; i<ndims; i++ ) {
		s_start[i] = (size_t)start[i];
		s_count[i] = (size_t)count[i];
		}

	*retval = nc_put_vara_int(*ncid, *varid, s_start, s_count, data );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_put_vara_int: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_put_var_int( int *ncid, int *varid, int *data, int *retval )
{
	*retval = nc_put_var_int(*ncid, *varid, data );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_put_var_int: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_put_var_double( int *ncid, int *varid, double *data, int *retval )
{
	*retval = nc_put_var_double(*ncid, *varid, data );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_put_var_double: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_def_var_int( int *ncid, char **varname, int *ndims, int *dimids, 
	int *varid, int *retval )
{
	*retval = nc_def_var(*ncid, varname[0], 
		NC_INT, *ndims, dimids, varid );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_def_var_int: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_def_var_short( int *ncid, char **varname, int *ndims, int *dimids, 
	int *varid, int *retval )
{
	*retval = nc_def_var(*ncid, varname[0], 
		NC_SHORT, *ndims, dimids, varid );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_def_var_int: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_def_var_float( int *ncid, char **varname, int *ndims, int *dimids, 
	int *varid, int *retval )
{
	*retval = nc_def_var(*ncid, varname[0], 
		NC_FLOAT, *ndims, dimids, varid );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_def_var_float: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_def_var_double( int *ncid, char **varname, int *ndims, int *dimids, 
	int *varid, int *retval )
{
	*retval = nc_def_var(*ncid, varname[0], 
		NC_DOUBLE, *ndims, dimids, varid );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_def_var_double: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_def_dim( int *ncid, char **dimname, int *size, int *dimid, 
	int *retval )
{
	*retval = nc_def_dim(*ncid, dimname[0], 
		*size, dimid );
	if( *retval != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_def_dim: %s\n", 
			nc_strerror(*retval) );
}

/*********************************************************************/
void R_nc_redef( int *ncid )
{
	int	err;
	err = nc_redef(*ncid);
	if( err != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_redef: %s\n", 
			nc_strerror(err) );
}

/*********************************************************************/
void R_nc_enddef( int *ncid )
{
	int	err;
	err = nc_enddef(*ncid);
	if( err != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_enddef: %s\n", 
			nc_strerror(err) );
}

/*********************************************************************/
void R_nc_sync( int *ncid )
{
	int	err;
	err = nc_sync(*ncid);
	if( err != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_sync: %s\n", 
			nc_strerror(err) );
}

/*********************************************************************/
void R_nc_close( int *ncid )
{
	int	err;
	err = nc_close(*ncid);
	if( err != NC_NOERR ) 
		fprintf( stderr, "Error in R_nc_close: %s\n", 
			nc_strerror(err) );
}

