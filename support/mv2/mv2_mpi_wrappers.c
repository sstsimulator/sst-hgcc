/* Global MPI_* wrappers for macOS bundle (weak PMPI_* stays local); hgcc_* stubs for tests. */
#include <string.h>
#include <stdio.h>

#define HGCC_NO_REPLACEMENTS 1
#include <unistd.h>

int hgcc_gethostname(const char* name, size_t sz)
{
    snprintf((char*)name, sz, "ssthost");
    return 0;
}

long hgcc_gethostid(void)
{
    return 0;
}

int hgcc_usleep(unsigned usecs)
{
    (void)usecs;
    return 0;
}

#undef HGCC_NO_REPLACEMENTS
#include <mpi.h>

int MPI_Abort(MPI_Comm c, int e)                               { return PMPI_Abort(c, e); }
int MPI_Alloc_mem(MPI_Aint s, MPI_Info i, void *p)             { return PMPI_Alloc_mem(s, i, p); }
int MPI_Allreduce(const void *s, void *r, int c,
                  MPI_Datatype d, MPI_Op o, MPI_Comm cm)        { return PMPI_Allreduce(s, r, c, d, o, cm); }
int MPI_Barrier(MPI_Comm c)                                     { return PMPI_Barrier(c); }
int MPI_Comm_dup(MPI_Comm c, MPI_Comm *n)                      { return PMPI_Comm_dup(c, n); }
int MPI_Comm_free(MPI_Comm *c)                                  { return PMPI_Comm_free(c); }
int MPI_Comm_rank(MPI_Comm c, int *r)                           { return PMPI_Comm_rank(c, r); }
int MPI_Comm_size(MPI_Comm c, int *s)                           { return PMPI_Comm_size(c, s); }
int MPI_Finalize(void)                                          { return PMPI_Finalize(); }
int MPI_Free_mem(void *b)                                       { return PMPI_Free_mem(b); }
int MPI_Get(void *o, int oc, MPI_Datatype od, int tr,
            MPI_Aint td, int tc, MPI_Datatype tt, MPI_Win w)   { return PMPI_Get(o, oc, od, tr, td, tc, tt, w); }
int MPI_Init(int *argc, char ***argv)                           { return PMPI_Init(argc, argv); }
int MPI_Put(const void *o, int oc, MPI_Datatype od, int tr,
            MPI_Aint td, int tc, MPI_Datatype tt, MPI_Win w)   { return PMPI_Put(o, oc, od, tr, td, tc, tt, w); }
int MPI_Recv(void *b, int c, MPI_Datatype d, int s,
             int t, MPI_Comm cm, MPI_Status *st)                { return PMPI_Recv(b, c, d, s, t, cm, st); }
int MPI_Send(const void *b, int c, MPI_Datatype d,
             int dest, int t, MPI_Comm cm)                      { return PMPI_Send(b, c, d, dest, t, cm); }
int MPI_Testsome(int ic, MPI_Request *r, int *oc,
                 int *idx, MPI_Status *st)                      { return PMPI_Testsome(ic, r, oc, idx, st); }
int MPI_Win_create(void *b, MPI_Aint s, int d,
                   MPI_Info i, MPI_Comm c, MPI_Win *w)          { return PMPI_Win_create(b, s, d, i, c, w); }
int MPI_Win_free(MPI_Win *w)                                    { return PMPI_Win_free(w); }
int MPI_Win_lock(int lt, int r, int a, MPI_Win w)              { return PMPI_Win_lock(lt, r, a, w); }
int MPI_Win_unlock(int r, MPI_Win w)                            { return PMPI_Win_unlock(r, w); }
