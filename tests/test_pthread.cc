#define ssthg_app_name test_pthread
#include <mercury/common/skeleton.h>

#include <pthread.h>
#include <iostream>

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_cond  = PTHREAD_COND_INITIALIZER;
static pthread_barrier_t g_barrier;
static int g_shared_counter = 0;
static int g_cond_flag = 0;

static pthread_key_t g_tls_key;

static pthread_spinlock_t g_spin;

void* worker(void* arg)
{
  int id = *(int*)arg;

  /* TLS: each thread stores its own id */
  int* my_id = new int(id);
  pthread_setspecific(g_tls_key, my_id);

  /* Mutex: safely increment shared counter */
  pthread_mutex_lock(&g_mutex);
  g_shared_counter++;
  std::cout << "thread " << id << ": counter=" << g_shared_counter << std::endl;
  pthread_mutex_unlock(&g_mutex);

  /* Spinlock: quick increment */
  pthread_spin_lock(&g_spin);
  g_shared_counter++;
  pthread_spin_unlock(&g_spin);

  /* Barrier: all threads synchronize here */
  int brc = pthread_barrier_wait(&g_barrier);
  if (brc == PTHREAD_BARRIER_SERIAL_THREAD) {
    std::cout << "thread " << id << ": was the barrier serial thread" << std::endl;
  }

  /* Verify TLS survived the barrier */
  int* stored = (int*)pthread_getspecific(g_tls_key);
  if (stored == nullptr || *stored != id) {
    std::cout << "thread " << id << ": TLS MISMATCH" << std::endl;
  } else {
    std::cout << "thread " << id << ": TLS ok (val=" << *stored << ")" << std::endl;
  }

  /* Condition variable: last thread signals the main thread */
  pthread_mutex_lock(&g_mutex);
  g_cond_flag++;
  if (g_cond_flag == 2) {
    pthread_cond_signal(&g_cond);
  }
  pthread_mutex_unlock(&g_mutex);

  delete my_id;
  return nullptr;
}

int main(int argc, char* argv[])
{
  const int NUM_THREADS = 2;
  pthread_t threads[NUM_THREADS];
  int ids[NUM_THREADS];

  pthread_key_create(&g_tls_key, nullptr);
  pthread_spin_init(&g_spin, 0);
  pthread_barrier_init(&g_barrier, nullptr, NUM_THREADS);

  for (int i = 0; i < NUM_THREADS; i++) {
    ids[i] = i;
    pthread_create(&threads[i], nullptr, worker, &ids[i]);
  }

  /* Wait for both threads to signal via condvar */
  pthread_mutex_lock(&g_mutex);
  while (g_cond_flag < NUM_THREADS) {
    pthread_cond_wait(&g_cond, &g_mutex);
  }
  pthread_mutex_unlock(&g_mutex);

  /* Join all threads */
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], nullptr);
  }

  std::cout << "final counter=" << g_shared_counter
            << " (expected " << NUM_THREADS * 2 << ")" << std::endl;

  pthread_barrier_destroy(&g_barrier);
  pthread_spin_destroy(&g_spin);
  pthread_key_delete(g_tls_key);

  return 0;
}
