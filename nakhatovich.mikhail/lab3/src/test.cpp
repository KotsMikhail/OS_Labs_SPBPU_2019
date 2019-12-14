#include <pthread.h>
#include <stdio.h>
#include <atomic>

#include <numeric>

#include "set_fine.h"
#include "set_optimistic.h"
#include "test.h"

struct test_info_t 
{
    set_t<int> *set;
    vector_int_t &data;
    bool *run;

    size_t *arr;
    bool *run_writers;

    test_info_t(set_t<int> *set, vector_int_t &data, bool *run) 
        : set(set), data(data), run(run), arr(nullptr), run_writers(nullptr)
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
    size_t len = ti->data.size();
    while (!*ti->run) {};
    for (size_t i = 0; i < len; ++i)
        set->add(ti->data[i]);
    pthread_exit(nullptr);
}

void * read(void *args)
{
    test_info_t * ti = (test_info_t *)(args);
    set_t<int> * set = ti->set;
    size_t len = ti->data.size();
    while (!*ti->run) {};
    for (size_t i = 0; i < len; ++i)
        set->remove(ti->data[i]);
    pthread_exit(nullptr);
}

void * read_w(void *args)
{
    test_info_t * ti = (test_info_t *)(args);
    set_t<int> * set = ti->set;
    size_t val, len = ti->data.size(), miss;
    int value;
    while (!*ti->run) {};
    for (size_t i = 0; i < len; ++i)
    {
        value = ti->data[i];
        miss = 0;
        while (!(set->remove(value)))
        {
            if (!*ti->run_writers && miss++ >= 1)
                pthread_exit(nullptr); 
            pthread_yield();
        }
        do
            val = ti->arr[value];
        while (!__sync_bool_compare_and_swap(ti->arr + value, val, val + 1));
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

bool check_common(size_t *data, size_t cnt_elements)
{
    bool ret = true;
    for (size_t i = 0; i < cnt_elements; ++i)
    {
        if (data[i] != 1)
        {
            printf("common: value for %lu is %lu\n", i, data[i]);
            ret = false;
        }
    }
    return ret;
}

void delete_test_info(vector_ti_t &data)
{
    for (test_info_t *elem : data)
        delete elem;
}

void join_threads(vector_pthread_t &threads, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        pthread_join(threads[i], nullptr);
}

size_t create_threads(vector_pthread_t &threads, void *(*func)(void*), vector_ti_t &tis)
{
    printf("Create threads.\n");
    for (size_t i = 0; i < threads.size(); ++i)
    {
        if (pthread_create(&(threads[i]), nullptr, func, tis[i]))
        {
            printf("Couldn't create thread #%lu.\n%lu/%lu threads was created.\n", i, i, threads.size());
            return i;
        }
    }
    printf("%lu/%lu threads was created.\n", threads.size(), threads.size());
    return threads.size();
}

bool create_data_sets(set_t<int> *set, size_t cnt_threads, const vector_size_t &cnt_elements, vvector_int_t &data_sets, vector_ti_t &tis, bool *run, bool add=false)
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
                delete_test_info(tis);
                printf("not enouh memory\n");
                return false;
            }
        }
        tis[i] = new (std::nothrow) test_info_t(set, data_sets[i], run);
        if (!tis[i])
        {
            delete_test_info(tis);
            printf("not enouh memory\n");
            return false;
        }
        /*
        if (n > 0)
            printf("Data set #%lu: [%d..%d] with size %lu.\n", i, data_sets[i][0], data_sets[i][n - 1], n);
        else
            printf("Data set #%lu: size 0.\n", i);
        */
    }
    return true;
}

void run_writers_test(set_t<int> *set, size_t cnt_writers, const vector_size_t &cnt_records, bool check, double *time)
{
    vector_ti_t tis(cnt_writers, nullptr);
    vector_pthread_t tids(cnt_writers);
    vvector_int_t data_sets(cnt_writers);
    bool run = false;
    if (!create_data_sets(set, cnt_writers, cnt_records, data_sets, tis, &run))
        return;
    size_t n = create_threads(tids, write, tis);
    run = true;
    join_threads(tids, n);
    delete_test_info(tis);
    if (n == cnt_writers && check && check_writers(set, data_sets))
        printf("Succesfully.\n");
}

void run_readers_test(set_t<int> *set, size_t cnt_readers, const vector_size_t &cnt_readings, bool check, double *time)
{
    vector_ti_t tis(cnt_readers, nullptr);
    vector_pthread_t tids(cnt_readers);
    vvector_int_t data_sets(cnt_readers);
    bool run = false;
    if (!create_data_sets(set, cnt_readers, cnt_readings, data_sets, tis, &run, true))
        return;
    size_t n = create_threads(tids, read, tis);
    run = true;
    join_threads(tids, n);
    delete_test_info(tis);
    if (n == cnt_readers && check && check_readers(set, data_sets))
        printf("Succesfully.\n");
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
    size_t * data = new (std::nothrow) size_t[cnt_elements]();
    get_count_elements_test(cnt_readers, cnt_elements, cnt_readings);
    get_count_elements_test(cnt_writers, cnt_elements, cnt_records);
    bool run = false, run_writers = true;
    if (!create_data_sets(set, cnt_readers, cnt_readings, rdata_sets, rtis, &run))
        return;
    for (test_info_t * ti : rtis)
    {        
        ti->arr = data;
        ti->run_writers = &run_writers;
    }
    if (!create_data_sets(set, cnt_writers, cnt_records, wdata_sets, wtis, &run))
    {
        delete_test_info(rtis);
        delete[] data;
        return;
    }
    size_t wn = create_threads(wtids, write, wtis);
    size_t rn = create_threads(rtids, read_w, rtis);
    run = true;
    join_threads(wtids, wn);
    run_writers = false;
    join_threads(rtids, rn);
    delete_test_info(wtis);
    delete_test_info(rtis);
    if (wn == cnt_writers && rn == cnt_readers && check && check_common(data, cnt_elements))
        printf("Succesfully.\n");
    delete[] data;
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
