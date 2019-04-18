//
//  main.c
//  AVL
//
//  Created by vector on 2018/11/30.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include <stdio.h>

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}




////新增加一个元素
//void shiftup(int i) { //传入一个需要向上调整的结点编号i
//    int flag = 0; //用来标记是否需要继续向上调整
//    if (i == 1) return ; //如果是堆顶，就返回，不需要再调整了
//    //不在堆顶，并且当前结点i的值比父节点小的时候就继续向上调整
//    while (i != 1 && flag == 0) {
//        //判断是否比父节点的小
//        if (h[i] < h[i/2]) {
//            swap(i, i/2);
//        } else{
//            flag = 1;
//        }
//        i = i/2;
//    }
//}

//void shiftdown(int i) { //传入一个需要向下调整的结点编号i
//    int t, flag = 0; //flag用来标记是否需要继续向下调整
//    while (i * 2 <= n && flag == 0) {
//        //首先判断它和左儿子的关系，并用t记录值较小的节点编号
//        if (h[i] > h[i*2]) {
//            t = i*2;
//        } else {
//            t = i;
//        }
//
//        //如果它有右儿子，再对右儿子进行讨论
//        if (i*2 + 1 <= n) {
//            //如果它的右儿子的值更小，更新较小的结点编号
//            if (h[t] < h[i*2 + 1])
//                t = i * 2 + 1;
//        }
//
//        //如果发现最小的编号不是自己，说明子结点中有比父节点更小的
//        if (t != i) {
//            swap(t, i);
//            i = t;
//        } else {
//            flag = 1;
//        }
//    }
//}

/**
 *  将start到end的数据都往下沉
 *  将子结点和父结点进行比较
 *
 *  @param start <#start description#>
 *  @param end   <#end description#>
 */
void MinHeap::shiftDown(int start, int end) {
    
}

/**
 *  将end到0的数据都上浮
 *
 *  @param end
 */
void MinHeap::shiftUp(int end) {
    int i = end;
    while (i > 0) {
        int parent = i % 2 == 0 ? i/2 - 1 : i/2;
        if (heap[i] < heap[parent]) {
            swap(&heap[i], &heap[parent]);
            i = parent;
        }
        else {
            --i;
        }
    }
}

/**
 *  删除最小值，也就是堆顶，用最大值替代，并且下沉
 *
 *  @return <#return value description#>
 */
bool MinHeap::removeMin() {
    if (currentSize == 0) {
        Log("current size is 0!");
        return false;
    }
    heap[0] = heap[currentSize-1];
    --currentSize;
    shiftDown(0, currentSize-1);
    return true;
}

/**
 *  插入值后要进行上浮
 *
 *  @param value <#value description#>
 *
 *  @return <#return value description#>
 */
bool MinHeap::insert(const int value) {
    if (currentSize == maxSize) {
        int *nHeap = heap;
        heap = new int(maxSize + 10);
        for (int i = 0; i < maxSize; ++i) {
            heap[i] = nHeap[i];
        }
        delete [] nHeap;
        Log("heap is resize!");
    }
    heap[currentSize++] = value;
    shiftUp(currentSize-1);
    return true;
}

/**
 *  取得堆顶值
 *
 *  @return <#return value description#>
 */
int MinHeap::poll() {
    if (isEmpty()) {
        Log("heap is null!");
        return -1;
    }
    return heap[0];
}

int main(int argc, const char * argv[]) {
    // insert code here...
    
    MinHeap Heap;
    
    Log("------------Insert------------");
    int arr[8] = {8, 7, 6, 5, 4, 3, 2, 1};
    for (int i = 0; i < 8; i++) {
        Heap.insert(arr[i]);
    }
    
    Heap.toString();
    
    Log("------------Remove------------");
    Heap.removeMin();
    
    Heap.toString();
    
    Log("------------Get------------");
    std::cout << "poll:" << Heap.poll() << std::endl;
    
    return 0;
}
