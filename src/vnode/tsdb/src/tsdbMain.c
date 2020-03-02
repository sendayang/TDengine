#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// #include "taosdef.h"
// #include "disk.h"
#include "tsdb.h"
#include "tsdbCache.h"
#include "tsdbFile.h"
#include "tsdbMeta.h"

enum { TSDB_REPO_STATE_ACTIVE, TSDB_REPO_STATE_CLOSED, TSDB_REPO_STATE_CONFIGURING };

typedef struct _tsdb_repo {
  // TSDB configuration
  STsdbCfg config;

  // The meter meta handle of this TSDB repository
  SMetaHandle *tsdbMeta;

  // The cache Handle
  SCacheHandle *tsdbCache;

  // Disk tier handle for multi-tier storage
  void *diskTier;

  // File Store
  void *tsdbFiles;

  pthread_mutex_t tsdbMutex;

  // A limiter to monitor the resources used by tsdb
  void *limiter;

  int8_t state;

} STsdbRepo;

static int32_t tsdbCheckAndSetDefaultCfg(STsdbCfg *pCfg);
static int32_t tsdbCreateRepoFiles(STsdbRepo *pRepo);

#define TSDB_GET_TABLE_BY_ID(pRepo, sid) (((STSDBRepo *)pRepo)->pTableList)[sid]
#define TSDB_GET_TABLE_BY_NAME(pRepo, name)
#define TSDB_IS_REPO_ACTIVE(pRepo) ((pRepo)->state == TSDB_REPO_STATE_ACTIVE)
#define TSDB_IS_REPO_CLOSED(pRepo) ((pRepo)->state == TSDB_REPO_STATE_CLOSED)

tsdb_repo_t *tsdbCreateRepo(char *rootDir, STsdbCfg *pCfg, void *limiter) {

  if (rootDir == NULL) return NULL;

  if (access(rootDir, F_OK|R_OK|W_OK) == -1) return NULL;

  if (tsdbCheckAndSetDefaultCfg(pCfg) < 0) {
    return NULL;
  }

  STsdbRepo *pRepo = (STsdbRepo *)malloc(sizeof(STsdbRepo));
  if (pRepo == NULL) {
    return NULL;
  }

  pRepo->config = *pCfg;
  pRepo->limiter = limiter;

  pRepo->tsdbMeta = tsdbCreateMeta(pCfg->maxTables);
  if (pRepo->tsdbMeta == NULL) {
    // TODO: deal with error
    free(pRepo);
    return NULL;
  }

  // TODO: Initialize cache handle
  pRepo->tsdbCache = tsdbCreateCache(5);
  if (pRepo->tsdbCache == NULL) {
    // TODO: free the object and return error
    tsdbFreeMetaHandle(pRepo->tsdbCache);
    free(pRepo);
    return NULL;
  }

  // Create the Meta data file and data directory
  if (tsdbCreateRepoFiles(pRepo) < 0) {
    // Failed to create and save files
    tsdbFreeMetaHandle(pRepo->tsdbCache);
    free(pRepo);
    return NULL;
  }

  pRepo->state = TSDB_REPO_STATE_ACTIVE;

  return (tsdb_repo_t *)pRepo;
}

int32_t tsdbDropRepo(tsdb_repo_t *repo) {
  STsdbRepo *pRepo = (STsdbRepo *)repo;

  pRepo->state = TSDB_REPO_STATE_CLOSED;

  // Free the metaHandle
  tsdbFreeMetaHandle(pRepo->tsdbMeta);

  // Free the cache
  tsdbFreeCache(pRepo->tsdbCache);

  tsdbClearFiles(pRepo);

  return 0;
}

tsdb_repo_t *tsdbOpenRepo(char *tsdbDir) {
  if (access(tsdbDir, F_OK | W_OK | R_OK) < 0) {
    return NULL;
  }

  STsdbRepo *pRepo = (STsdbRepo *)malloc(sizeof(STsdbRepo));
  if (pRepo == NULL) {
    return NULL;
  }

  // TODO: Initialize configuration from the file
  pRepo->tsdbMeta = tsdbOpenMetaHandle();
  if (pRepo->tsdbMeta == NULL) {
    free(pRepo);
    return NULL;
  }

  pRepo->tsdbCache = tsdbCreateCache(5);
  if (pRepo->tsdbCache == NULL) {
    // TODO: deal with error
    return NULL;
  }

  pRepo->state = TSDB_REPO_STATE_ACTIVE;

  return (tsdb_repo_t *)pRepo;
}

static int32_t tsdbFlushCache(STsdbRepo *pRepo) {
  // TODO
}

int32_t tsdbCloseRepo(tsdb_repo_t *repo) {
  STsdbRepo *pRepo = (STsdbRepo *)repo;

  tsdbFlushCache(pRepo);

  pRepo->state = TSDB_REPO_STATE_CLOSED;

  tsdbFreeMetaHandle(pRepo->tsdbMeta);

  tsdbFreeCache(pRepo->tsdbMeta);

  return 0;
}

int32_t tsdbConfigRepo(tsdb_repo_t *repo, STsdbCfg *pCfg) {
  STsdbRepo *pRepo = (STsdbRepo *)repo;

  pRepo->config = pCfg;
  // TODO
  return 0;
}

STSDBRepoInfo *tsdbGetStatus(tsdb_repo_t *pRepo) {
  // TODO
}

int32_t tsdbCreateTable(tsdb_repo_t *repo, STableCfg *pCfg) {
  STsdbRepo *pRepo = (STsdbRepo *)repo;
  return tsdbCreateTableImpl(pRepo->tsdbMeta, pCfg);
}

int32_t tsdbAlterTable(tsdb_repo_t *pRepo, STableCfg *pCfg) {
  // TODO
}

STableInfo *tsdbGetTableInfo(tsdb_repo_t *pRepo, STableId tid, int32_t *error) {
  // TODO
}

int32_t tsdbInsertData(tsdb_repo_t *pRepo, STableId tid, char *pData, int32_t *error) {
  // TODO
}

// Check the configuration and set default options
static int32_t tsdbCheckAndSetDefaultCfg(STsdbCfg *pCfg) {
  // TODO
  return 0;
}

static int32_t tsdbCreateRepoFiles(STsdbRepo *pRepo) {
  // TODO
}

static int32_t tsdbClearFiles(STsdbRepo *pRepo) {
  // TODO
}