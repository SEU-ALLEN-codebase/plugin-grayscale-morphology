#ifndef GSDT_H
#define GSDT_H

#include <v3d_interface.h>
#include <QVector3D>
#include <map>

#define INF 3.4e+38

/******************************************************************************
 * Fast marching based distance transformation
 *
 * Input : inimg1d     original input grayscale image
 *         cnn_type    the connection type
 *         bkg_thresh  the background threshold, less or equal then bkg_thresh will considered as background
 *
 * Output : phi       distance tranformed output image
 *
 * Notice :
 * 1. the input pixel number should not be larger than 2G if sizeof(long) == 4 //this should have been solved by changing "long" to "V3DLONG". see changes introduced in version 634
 * 2. The background point is of intensity 0
 * *****************************************************************************/

/// modified from v3d plugin gsdt by Hang Xiao
/// combined heap.h and fastmarching_dt.h
/// change pointers to shared pointer

using namespace std;


struct HeapElem
{
    int    heap_id;
    V3DLONG   img_ind;
    float value;
    HeapElem(V3DLONG _ind, float _value)
    {
        heap_id = -1;
        img_ind = _ind;
        value = _value;
    }
};


class Heap       // Basic Min heap
{
public:
    Heap()
    {
        elems.reserve(10000);
    }
    QSharedPointer<HeapElem> delete_min()
    {
        if (elems.empty())
            throw "GSDT: Heap empty";
        auto min_elem = elems[0];

        if (elems.size() == 1) elems.clear();
        else
        {
            elems[0] = elems[elems.size() - 1];
            elems[0]->heap_id = 0;
            elems.erase(elems.begin() + elems.size() - 1);
            down_heap(0);
        }
        return min_elem;
    }
    void insert(const QSharedPointer<HeapElem>& t)
    {
        elems.push_back(t);
        if (t.isNull())
            throw "GSDT: Heap element null.";
        t->heap_id = elems.size() - 1;
        up_heap(t->heap_id);
    }
    bool empty() {return elems.empty();}
    void adjust(int id, float new_value)
    {
        if (elems[id].isNull())
            throw "GSDT: Heap element null.";
        float old_value = elems[id]->value;
        elems[id]->value = new_value;
        if (new_value < old_value) up_heap(id);
        else if (new_value > old_value) down_heap(id);
    }
private:
    vector<QSharedPointer<HeapElem>> elems;
    bool swap_heap(int id1, int id2)
    {
        if(id1 < 0 || id1 >= elems.size() || id2 < 0 || id2 >= elems.size()) return false;
        if(id1 == id2) return false;
        int pid = id1 < id2 ? id1 : id2;
        int cid = id1 > id2 ? id1 : id2;
        if (cid != 2*(pid+1) -1 && cid != 2*(pid+1))
            throw "GSDT: heap element not connected.";
        if (elems[pid].isNull() || elems[cid].isNull())
            throw "GSDT: Heap element null.";
        if(elems[pid]->value <= elems[cid]->value) return false;
        else
        {
            auto tmp = elems[pid];
            elems[pid] = elems[cid];
            elems[cid] = tmp;
            elems[pid]->heap_id = pid;
            elems[cid]->heap_id = cid;
            return true;
        }
    }
    void up_heap(int id)
    {
        int pid = (id+1)/2 - 1;
        if(swap_heap(id, pid)) up_heap(pid);
    }
    void down_heap(int id)
    {
        int cid1 = 2*(id+1) -1;
        int cid2 = 2*(id+1);
        if(cid1 >= elems.size()) return;
        else if(cid1 == elems.size() - 1)
        {
            swap_heap(id, cid1);
        }
        else if(cid1 < elems.size() - 1)
        {
            if (elems[cid1].isNull() || elems[cid2].isNull())
                throw "GSDT: Heap element null.";
            int cid = elems[cid1]->value < elems[cid2]->value ? cid1 : cid2;
            if(swap_heap(id, cid)) down_heap(cid);
        }
    }
};

template<class T>
void fastmarching_dt(T* inimg1d, v3d_float32* dst, const V3DLONG sz[4], int cnn_type, float bkg_thresh, float z_thickness)
{
    enum {ALIVE = -1, TRIAL = 0, FAR = 1};

    auto&& tol_sz = sz[0] * sz[1] * sz[2];
    auto&& sz01 = sz[0] * sz[1];

    QSharedPointer<char> state_buffer(new char[tol_sz], default_delete<char[]>());
    if (state_buffer.isNull())
        throw "GSDT: Memory allocation failed.";
    auto&& state = state_buffer.data();
    V3DLONG bkg_count = 0;                          // for process counting
    V3DLONG bdr_count = 0;                          // for process counting
    for(V3DLONG i = 0; i < tol_sz; ++i)
        if(inimg1d[i] <= bkg_thresh)
        {
            dst[i] = inimg1d[i];
            state[i] = ALIVE;
            ++bkg_count;
        }
        else
        {
            dst[i] = INF;
            state[i] = FAR;
        }

    Heap heap;
    map<V3DLONG, QSharedPointer<HeapElem>> elems;

    // init heap
    {
        V3DLONG i = -1, j = -1, k = -1;
        for(V3DLONG ind = 0; ind < tol_sz; ind++)
        {
            i++; if(i%sz[0] == 0){i=0; j++; if(j%sz[1]==0){j=0; k++;}}
            if(state[ind] == ALIVE)
            {
                for(int kk = -1; kk <= 1; kk++)
                {
                    V3DLONG k2 = k+kk;
                    if(k2 < 0 || k2 >= sz[2]) continue;
                    for(int jj = -1; jj <= 1; jj++)
                    {
                        V3DLONG j2 = j+jj;
                        if(j2 < 0 || j2 >= sz[1]) continue;
                        for(int ii = -1; ii <=1; ii++)
                        {
                            V3DLONG i2 = i+ii;
                            if(i2 < 0 || i2 >= sz[0]) continue;
                            int offset = std::abs(ii) + std::abs(jj) + std::abs(kk);
                            if(offset == 0 || offset > cnn_type) continue;
                            V3DLONG ind2 = k2 * sz01 + j2 * sz[0] + i2;
                            if(state[ind2] == FAR)
                            {
                                V3DLONG min_ind = ind;
                                // get minimum Alive point around ind2
                                if(dst[min_ind] > 0.0)
                                {
                                    for(int kkk = -1; kkk <= 1; kkk++)
                                    {
                                        V3DLONG k3 = k2 + kkk;
                                        if(k3 < 0 || k3 >= sz[2]) continue;
                                        for(int jjj = -1; jjj <= 1; jjj++)
                                        {
                                            V3DLONG j3 = j2 + jjj;
                                            if(j3 < 0 || j3 >= sz[1]) continue;
                                            for(int iii = -1; iii <= 1; iii++)
                                            {
                                                V3DLONG i3 = i2 + iii;
                                                if(i3 < 0 || i3 >= sz[0]) continue;
                                                int offset2 = std::abs(iii) + std::abs(jjj) + std::abs(kkk);
                                                if(offset2 == 0 || offset2 > cnn_type) continue;
                                                V3DLONG ind3 = k3 * sz01 + j3 * sz[0] + i3;
                                                if(state[ind3] == ALIVE && dst[ind3] < dst[min_ind]) min_ind = ind3;
                                            }
                                        }
                                    }
                                }
                                // over
                                dst[ind2] = dst[min_ind] + inimg1d[ind2];
                                state[ind2] = TRIAL;
                                elems[ind2] = QSharedPointer<HeapElem>::create(ind2, dst[ind2]);
                                if (elems[ind2].isNull())
                                    throw "GSDT: Memory allocation failed.";
                                heap.insert(elems[ind2]);
                                ++bdr_count;
                            }
                        }
                    }
                }
            }
        }
    }

    qDebug() << "bkg_count =" << bkg_count << "(" << bkg_count/(float)tol_sz << ")";
    qDebug() << "bdr_count =" << bdr_count << "(" << bdr_count/(float)tol_sz << ")";
    qDebug() << "elems.size() =" << elems.size();

    // loops
    bool f = false;
    while(!heap.empty())
    {
        auto&& min_elem = heap.delete_min();
        elems.erase(min_elem->img_ind);

        V3DLONG min_ind = min_elem->img_ind;

        state[min_ind] = ALIVE;
        V3DLONG i = min_ind % sz[0];
        V3DLONG j = (min_ind/sz[0]) % sz[1];
        V3DLONG k = (min_ind/sz01) % sz[2];

        int w, h, d;
        for(int kk = -1; kk <= 1; kk++)
        {
            d = k+kk;
            if(d < 0 || d >= sz[2]) continue;
            for(int jj = -1; jj <= 1; jj++)
            {
                h = j+jj;
                if(h < 0 || h >= sz[1]) continue;
                for(int ii = -1; ii <= 1; ii++)
                {
                    w = i+ii;
                    if(w < 0 || w >= sz[0]) continue;
                    int offset = std::abs(ii) + std::abs(jj) + std::abs(kk);
                    if(offset == 0 || offset > cnn_type) continue;
                    V3DLONG index = d*sz01 + h*sz[0] + w;

                    if(state[index] != ALIVE)
                    {
                        auto&& new_dist = dst[min_ind] + inimg1d[index] * QVector3D(ii, jj, kk * z_thickness).length();
                        if(state[index] == FAR)
                        {
                            dst[index] = new_dist;
                            elems[index] = QSharedPointer<HeapElem>::create(index, dst[index]);
                            heap.insert(elems[index]);
                            state[index] = TRIAL;
                        }
                        else if(state[index] == TRIAL && dst[index] > new_dist)
                        {
                            dst[index] = new_dist;
                            heap.adjust(elems[index]->heap_id, dst[index]);
                        }
                    }
                }
            }
        }
    }
}

#endif // GSDT_H
