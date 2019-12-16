#include <pthread.h>
#include <stdio.h>

#include <atomic>
#include <algorithm>
#include <numeric>
#include <random> 

#include "config.h"
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
using cntr_data_fn = bool(*)(set_t<int> *,size_t,const vector_size_t &,vvector_int_t &,vector_ti_t &,bool *,bool);
using simple_test_fn = bool(*)(set_t<int> *, cntr_data_fn, bool, double *);

void print_start(const char *name)
{
    printf("Test for %s started.\n", name);
}

void print_stop(const char *name)
{
    printf("Test for %s stopped.\n", name);
}

void get_start_time(struct timespec *start_time)
{
    clock_gettime(CLOCK_REALTIME, start_time);
}

void get_result_time(const struct timespec &start_time, double *time)
{
    struct timespec cur_time;
    clock_gettime(CLOCK_REALTIME, &cur_time);
    __time_t sec = cur_time.tv_sec - start_time.tv_sec;
    __syscall_slong_t nsec = cur_time.tv_nsec - start_time.tv_nsec;
    *time = sec + (double) nsec / 1e9;
}
/*
void get_prio_pol(int *policy, int *prio)
{
    struct sched_param sp;
    pthread_getschedparam(pthread_self(), policy, &sp);
    *prio = sp.sched_priority;
}
*/
void * write(void *args)
{
    test_info_t * ti = (test_info_t *)(args);
    set_t<int> * set = ti->set;
    // int policy, prio;
    // get_prio_pol(&policy, &prio);
    while (!*ti->run) {};
    // printf("prio = %d, [%d..%d], pol = %d\n", prio, sched_get_priority_min(policy), sched_get_priority_max(policy), policy);
    for (int value : ti->data)
        set->add(value);
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
    size_t val, miss;
    // int policy, prio;
    // get_prio_pol(&policy, &prio);
    while (!*ti->run) {};
    // printf("prio = %d, [%d..%d], pol = %d\n", prio, sched_get_priority_min(policy), sched_get_priority_max(policy), policy);
    for (int value : ti->data)
    {
        miss = 0;
        while (!set->remove(value))
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
    bool ret = set->empty();
    if (!ret)
        for (vector_int_t data_set : data)
            for (int value : data_set)
                if (set->contains(value))
                    printf("readers: set contains value: %d\n", value);
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

size_t create_threads(vector_pthread_t &threads, void *(*func)(void*), vector_ti_t &tis, bool is_print)
{
    if (is_print)
        printf("Create threads.\n");
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    struct sched_param sp;
    int sched = SCHED_RR;
    int min = sched_get_priority_min(sched), max = sched_get_priority_max(sched);
    size_t m = threads.size();
    sp.sched_priority = max;
    pthread_setschedparam(pthread_self(), sched, &sp);
    sp.sched_priority = min;
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, sched);
    pthread_attr_setschedparam(&attr, &sp);
    for (size_t i = 0; i < m; ++i)
    {
        int s = pthread_create(&(threads[i]), &attr, func, tis[i]);
        if (s)
        {
            printf("Couldn't create thread #%lu, errno %d.\n", i, s);
            if (is_print)
                printf("%lu/%lu threads was created.\n", i, m);
            return i;
        }
    }
    if (is_print)
        printf("%lu/%lu threads was created.\n", m, m);
    return m;
}

bool create_data_sets(set_t<int> *set, size_t cnt_threads, const vector_size_t &cnt_elements, 
                      vvector_int_t &data_sets, vector_ti_t &tis, bool *run, bool add=false)
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

bool run_writers_test(set_t<int> *set, cntr_data_fn cntr_data, bool check, double *time)
{
    config_t *config = config_t::get_instance();
    size_t cnt_writers = config->get_value(WRITERS_TH), cnt_elements = config->get_value(WRITERS_NUM);
    vector_size_t cnt_records(cnt_writers);
    vector_ti_t tis(cnt_writers, nullptr);
    vector_pthread_t tids(cnt_writers);
    vvector_int_t data_sets(cnt_writers);
    bool run = false;
    struct timespec start_time;
    get_count_elements_test(cnt_writers, cnt_elements, cnt_records);
    if (!cntr_data(set, cnt_writers, cnt_records, data_sets, tis, &run, false))
        return false;
    size_t n = create_threads(tids, write, tis, check);
    if (time)
        get_start_time(&start_time);
    run = true;
    join_threads(tids, n);
    if (time)
        get_result_time(start_time, time);
    delete_test_info(tis);
    if (n != cnt_writers)
        return false;
    if (check && check_writers(set, data_sets))
        printf("Succesfully.\n");
    return true;
}

bool run_readers_test(set_t<int> *set, cntr_data_fn cntr_data, bool check, double *time)
{
    config_t *config = config_t::get_instance();
    size_t cnt_readers = config->get_value(READERS_TH), cnt_elements = config->get_value(READERS_NUM);
    vector_size_t cnt_readings(cnt_readers);
    vector_ti_t tis(cnt_readers, nullptr);
    vector_pthread_t tids(cnt_readers);
    vvector_int_t data_sets(cnt_readers);
    bool run = false;
    struct timespec start_time;
    get_count_elements_test(cnt_readers, cnt_elements, cnt_readings);
    if (!cntr_data(set, cnt_readers, cnt_readings, data_sets, tis, &run, true))
        return false;
    size_t n = create_threads(tids, read, tis, check);
    if (time)
        get_start_time(&start_time);
    run = true;
    join_threads(tids, n);
    if (time)
        get_result_time(start_time, time);
    delete_test_info(tis);
    if (n != cnt_readers)
        return false;
    if (check && check_readers(set, data_sets))
        printf("Succesfully.\n");
    return true;
}

bool run_common_test(set_t<int> *set, cntr_data_fn cntr_data, bool check, double *time)
{
    config_t *config = config_t::get_instance();
    size_t cnt_writers = config->get_value(COMMON_WRITERS), cnt_readers = config->get_value(COMMON_READERS);
    size_t cnt_elements = config->get_value(COMMON_N);
    vector_size_t cnt_records(cnt_writers), cnt_readings(cnt_readers);
    vector_ti_t rtis(cnt_readers), wtis(cnt_writers);
    vector_pthread_t rtids(cnt_readers), wtids(cnt_writers);
    vvector_int_t rdata_sets(cnt_readers), wdata_sets(cnt_writers);
    size_t * data = new (std::nothrow) size_t[cnt_elements]();
    bool run = false, run_writers = true;
    struct timespec start_time;
    get_count_elements_test(cnt_readers, cnt_elements, cnt_readings);
    get_count_elements_test(cnt_writers, cnt_elements, cnt_records);
    if (!cntr_data(set, cnt_readers, cnt_readings, rdata_sets, rtis, &run, false))
        return false;
    for (test_info_t * ti : rtis)
    {        
        ti->arr = data;
        ti->run_writers = &run_writers;
    }
    if (!cntr_data(set, cnt_writers, cnt_records, wdata_sets, wtis, &run, false))
    {
        delete_test_info(rtis);
        delete[] data;
        return false;
    }
    size_t wn = create_threads(wtids, write, wtis, check);
    size_t rn = create_threads(rtids, read_w, rtis, check);
    if (time)
        get_start_time(&start_time);
    run = true;
    join_threads(wtids, wn);
    run_writers = false;
    join_threads(rtids, rn);
    if (time)
        get_result_time(start_time, time);
    delete_test_info(wtis);
    delete_test_info(rtis);
    if (wn != cnt_writers || rn != cnt_readers)
    {
        delete[] data;
        return false;
    }
    if (check && check_common(data, cnt_elements))
        printf("Succesfully.\n");
    delete[] data;
    return true;
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

void run_simple_test(set_type_t set_type, simple_test_type_t test_type)
{
    vector_string_t names = {"writers", "readers", "common"};
    simple_test_fn tests[] = {run_writers_test, run_readers_test, run_common_test};
    print_start(names[test_type].c_str());
    set_t<int> * set = create_set(set_type);
    if (set)
        tests[test_type](set, create_data_sets, true, nullptr);
    delete set;
    print_stop(names[test_type].c_str());
}

bool create_random_data_sets(set_t<int> *set, size_t cnt_threads, const vector_size_t &cnt_elements, 
                             vvector_int_t &data_sets, vector_ti_t &tis, bool *run, bool add=false)
{
    vector_int_t elements(std::accumulate(cnt_elements.begin(), cnt_elements.end(), 0));
    std::iota(elements.begin(), elements.end(), 0);
    std::shuffle(elements.begin(), elements.end(), std::default_random_engine(0));
    for (size_t i = 0, n = 0, k = 0; i < cnt_threads; ++i)
    {
        n = cnt_elements[i];
        data_sets[i].resize(n);
        for (size_t j = 0; j < n; ++j, ++k)
        {
            data_sets[i][j] = elements[k];
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
    }
    return true;
}

bool create_fixed_data_sets(set_t<int> *set, size_t cnt_threads, const vector_size_t &cnt_elements, 
                            vvector_int_t &data_sets, vector_ti_t &tis, bool *run, bool add=false)
{
    int elem = 0;
    size_t max_elems = *std::max_element(cnt_elements.begin(), cnt_elements.end());
    vector_size_t inds(cnt_threads, 0);
    for (size_t i = 0; i < cnt_threads; ++i)
        data_sets[i].resize(cnt_elements[i]);
    for (size_t i = 0; i < max_elems; ++i)
    {
        for (size_t j = 0; j < cnt_threads; ++j)
        {
            if (inds[j] != cnt_elements[j])
            {
                data_sets[j][inds[j]] = elem++;
                if (add && !set->add(data_sets[j][inds[j]]))
                {
                    delete_test_info(tis);
                    printf("not enouh memory\n");
                    return false;
                }
                ++inds[j];
            }
        }
    }
    for (size_t i = 0; i < cnt_threads; ++i)
    {
        tis[i] = new (std::nothrow) test_info_t(set, data_sets[i], run);
        if (!tis[i])
        {
            delete_test_info(tis);
            printf("not enouh memory\n");
            return false;
        }
    }
    return true;
}

bool run_time_test(set_type_t set_type, simple_test_fn test_func, cntr_data_fn cntr_data, double &time)
{
    config_t *config = config_t::get_instance();
    size_t iters = config->get_value(TIME_ITERATIONS), n = 0;
    double part_time;
    time = 0;
    for (size_t i = 0; i < iters; ++i)
    {
        set_t<int> * set = create_set(set_type);
        if (set && test_func(set, cntr_data, false, &part_time))
        {
            ++n;
            time += part_time;
        }
        delete set;
    }
    if (n > 0)
        time /= n;
    return (n > 0);
}

string_t get_set_name(set_type_t t)
{
    switch (t)
    {
    case SET_FINE:
        return string_t("Fine-Grained");
    case SET_OPTIMISTIC:
        return string_t("Optimistic");
    default:
        return string_t("");
    }
}

void run_time_test()
{
    print_start("time");
    simple_test_fn tests[] = {run_writers_test, run_readers_test, run_common_test};
    cntr_data_fn data_sets[] = { create_random_data_sets, create_fixed_data_sets };
    set_type_t sets[] = { SET_FINE, SET_OPTIMISTIC };
    double time;
    char line[120];
    printf("   set type   | writers random |  writers fixed | readers random |  readers fixed |  common random |  common fixed  |\n");
    printf("---------------------------------------------------------------------------------------------------------------------\n");
    for (set_type_t set_type : sets)
    {
        sprintf(line, "%13s |", get_set_name(set_type).c_str());
        for (size_t j = 0; j < 3; ++j)
        {
            simple_test_fn test_func = tests[j];
            for (size_t k = 0; k < 2; ++k)
            {
                cntr_data_fn data_set = data_sets[k];
                if (run_time_test(set_type, test_func, data_set, time))
                    sprintf(line + 15 + j * 34 + k * 17, "    %s    |", std::to_string(time).c_str());
                else
                    sprintf(line + 15 + j * 34 + k * 17, "       NA       |");
            }
        }
        printf("%s\n---------------------------------------------------------------------------------------------------------------------\n", line);
    }
    print_stop("time");
}
