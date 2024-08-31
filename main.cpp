void quicksort(std::vector<int>& array, int left, int right, std::atomic<int>* task_counter,
    std::promise<void>* completion_promise, ThreadPool& pool, bool make_thread) {
    if (left >= right) {
        if (--(*task_counter) == 0) {
            completion_promise->set_value();
        }
        return;
    }

    int i = left, j = right;
    int pivot = array[left + (right - left) / 2];

    while (i <= j) {
        while (array[i] < pivot) i++;
        while (array[j] > pivot) j--;
        if (i <= j) {
            std::swap(array[i], array[j]);
            i++;
            j--;
        }
    }

    if (make_thread && (right - left > 100000)) {
        task_counter->fetch_add(1);
        auto f = pool.push_task(quicksort, std::ref(array), left, j, task_counter, completion_promise, std::ref(pool), make_thread);
    }
    else {
        quicksort(array, left, j, task_counter, completion_promise, pool, false);
    }
    quicksort(array, i, right, task_counter, completion_promise, pool, false);

    if (--(*task_counter) == 0) {
        completion_promise->set_value();
    }
}
