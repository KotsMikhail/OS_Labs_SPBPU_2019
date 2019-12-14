#include <pthread.h>
#include <stdio.h>

#include <numeric>

#include "set_fine.h"
#include "set_optimistic.h"
#include "test.h"

struct test_info_t 
{
    set_t<int> *set;
    vector_int_t &data;

    vector_size_t *arr;
    pthread_mutex_t *lock;

    test_info_t(set_t<int> *set, vector_int_t &data) : set(set), data(data), arr(nullptr), lock(nullptr)
    {};
};

typedef std::vector<test_info_t*> vector_ti_t;

void print_start(const char *name)
{
    printf("Test for %s started.\n", name);
}

void print_stop(const char *name)
{
    printf("Test for %s stopped.\n", name);
}

void * write(void *args)
{
    test_info_t * ti = (test_info_t *)(args);
    set_t<int> * set = ti->set;
    for (int value : ti->data)
        set->add(value);
    pthread_exit(nullptr);
}

void * read(void *args)
{
    test_info_t * ti = (test_info_t *)(args);
    set_t<int> * set = ti->set;
    for (int value : ti->data)
        set->remove(value);
    pthread_exit(nullptr);
}

void * read_w(void *args)
{
    test_info_t * ti = (test_info_t *)(args);
    set_t<int> * set = ti->set;
    for (int value : ti->data)
    {
        while (!(set->remove(value))) 
            pthread_yield();
        pthread_mutex_lock(ti->lock);
        (*(ti->arr))[value] += 1;
        pthread_mutex_unlock(ti->lock);
    }
    pthread_exit(nullptr);
}

bool check_writers(set_t<int> *set, vvector_int_t &data)
{
    bool ret = true;
    for (vector_int_t data_set : data)
    {
        for (int value : data_set)
        {
            if (!set->contains(value))
            {
                printf("writers: set doesn't contain value: %d\n", value);
                ret = false;
            }
        }
    }
    return ret;
}

bool check_readers(set_t<int> *set, vvector_int_t &data)
{
    bool ret = true;
    for (vector_int_t data_set : data)
    {
        for (int value : data_set)
        {
            if (set->contains(value))
            {
                printf("readers: set contains value: %d\n", value);
                ret = false;
            }
        }
    }
    return ret;
}

bool check_common(vector_size_t &data)
{
    bool ret = true;
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (data[i] != 1)
        {
            printf("common: value for %lu is %lu\n", i, data[i]);
            ret = false;
        }
    }
    return ret;
}

template<class t>
void delete_all(std::vector<t*> &data)
{
    for (t *elem : data)
        delete elem;
}

void join_threads(vector_pthread_t &threads)
{
    for (pthread_t tid : threads)
        pthread_join(tid, nullptr);
}

void cancel_threads(vector_pthread_t &threads)
{
    for (pthread_t tid : threads)
        pthread_cancel(tid);    
}

bool create_threads(vector_pthread_t &threads, void *(*func)(void*), vector_ti_t &tis)
{
    for (size_t i = 0; i < threads.size(); ++i)
    {
        if (pthread_create(&(threads[i]), nullptr, func, tis[i]))
        {
            printf("Couldn't create thread #%lu.\n", i);
            delete_all(tis);
            cancel_threads(threads);
            return false;
        }
    }
    return true;
}

bool create_data_sets(set_t<int> *set, size_t cnt_threads, const vector_size_t &cnt_elements, vvector_int_t &data_sets, vector_ti_t &tis, bool add=false)
{
    int elem = 0;
    for (size_t i = 0, n = 0; i < cnt_threads; ++i)
    {
        n = cnt_elements[i];
        data_sets[i].resize(n);
        for (size_t j = 0; j < n; ++j)
        {
            data_sets[i][j] = elem++;
            if (add && !set->add(data_sets[i][j]))
            {
                delete_all(tis);
                printf("not enouh memory\n");
                return false;
            }
        }
        tis[i] = new (std::nothrow) test_info_t(set, data_sets[i]);
        if (!tis[i])
        {
            delete_all(tis);
            printf("not enouh memory\n");
            return false;
        }
        if (n > 0)
            printf("Data set #%lu: [%d..%d] with size %lu.\n", i, data_sets[i][0], data_sets[i][n - 1], n);
        else
            printf("Data set #%lu: size 0.\n", i);
    }
    return true;
}

void run_writers_test(set_t<int> *set, size_t cnt_writers, const vector_size_t &cnt_records, bool check, double *time)
{
    vector_ti_t tis(cnt_writers, nullptr);
    vector_pthread_t tids(cnt_writers);
    vvector_int_t data_sets(cnt_writers);
    if (!create_data_sets(set, cnt_writers, cnt_records, data_sets, tis))
        return;
    bool ret = create_threads(tids, write, tis);
    join_threads(tids);
    if (!ret)
        return;
    if (check && check_writers(set, data_sets))
        printf("Succesfully.\n");
    delete_all(tis);
}

void run_readers_test(set_t<int> *set, size_t cnt_readers, const vector_size_t &cnt_readings, bool check, double *time)
{
    vector_ti_t tis(cnt_readers, nullptr);
    vector_pthread_t tids(cnt_readers);
    vvector_int_t data_sets(cnt_readers);
    if (!create_data_sets(set, cnt_readers, cnt_readings, data_sets, tis, true))
        return;
    bool ret = create_threads(tids, read, tis);
    join_threads(tids);
    if (!ret)
        return;
    if (check && check_readers(set, data_sets))
        printf("Succesfully.\n");
    delete_all(tis);
}

void get_count_elements_test(size_t cnt_threads, size_t cnt_elements, vector_size_t &data)
{
    size_t i;
    if (cnt_elements <= cnt_threads)
    {
        for (i = 0; i < cnt_elements; ++i)
            data[i] = 1;
        for (; i < cnt_threads; ++i)
            data[i] = 0;
    }
    else
    {
        size_t n = cnt_elements / cnt_threads, r = cnt_elements - n * cnt_threads;
        for (i = 0; i < r; ++i)
            data[i] = n + 1;
        for (; i < cnt_threads; ++i)
            data[i] = n;
    }
}

void run_common_test(set_t<int> *set, size_t cnt_readers, size_t cnt_writers, size_t cnt_elements, bool check, double *time)
{
    std::vector<test_info_t*> rtis(cnt_readers), wtis(cnt_writers);
    std::vector<pthread_t> rtids(cnt_readers), wtids(cnt_writers);
    std::vector<vector_int_t> rdata_sets(cnt_readers), wdata_sets(cnt_writers);
    vector_size_t cnt_records(cnt_writers), cnt_readings(cnt_readers);
    vector_size_t data(cnt_elements, 0);
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    get_count_elements_test(cnt_readers, cnt_elements, cnt_readings);
    get_count_elements_test(cnt_writers, cnt_elements, cnt_records);
    if (!create_data_sets(set, cnt_readers, cnt_readings, rdata_sets, rtis))
        return;
    for (test_info_t * ti : rtis)
    {
        ti->arr = &data;
        ti->lock = &lock;
    }
    if (!create_data_sets(set, cnt_writers, cnt_records, wdata_sets, wtis))
    {
        delete_all(rtis);
        pthread_mutex_destroy(&lock);
        return;
    }
    if (!create_threads(wtids, write, wtis))
    {
        join_threads(wtids);
        return;
    }
    bool ret = create_threads(rtids, read_w, rtis);
    if (!ret)
        cancel_threads(wtids);
    join_threads(wtids);
    join_threads(rtids);
    if (!ret)
        return;
    if (check && check_common(data))
        printf("Succesfully.\n");
    delete_all(wtis);
    delete_all(rtis);
    pthread_mutex_destroy(&lock);
}

set_t<int> * create_set(set_type_t type)
{
    set_t<int> *set;
    switch (type) 
    {
    case SET_FINE:
        set = set_fine_t<int>::create_set();
        break;
    case SET_OPTIMISTIC:
        set = set_optimistic_t<int>::create_set();
        break;
    default:
        set = nullptr;
        break;
    }
    if (!set)
        printf("Couldn't create set.\n");
    return set;
}

void run_writers_test(set_type_t type, size_t cnt_writers, const vector_size_t &cnt_records)
{
    print_start("writers");
    set_t<int> * set = create_set(type);
    if (set)
        run_writers_test(set, cnt_writers, cnt_records, true, nullptr);
    delete set;
    print_stop("writers");
}

void run_readers_test(set_type_t type, size_t cnt_readers, const vector_size_t &cnt_readings)
{
    print_start("readers");
    set_t<int> * set = create_set(type);
    if (set)
        run_readers_test(set, cnt_readers, cnt_readings, true, nullptr);
    delete set;
    print_stop("readers");
}

void run_common_test(set_type_t type, size_t cnt_readers, size_t cnt_writers, size_t cnt_elements)
{
    print_start("common");
    set_t<int> * set = create_set(type);
    if (set)
        run_common_test(set, cnt_readers, cnt_writers, cnt_elements, true, nullptr);
    delete set;
    print_stop("common");
}

