/* Adapted from original code https://github.com/tmick0/generic_median/blob/master/MedianFilter.h
 */
#pragma once

template <size_t N>
class MedianFilter {
public:
  MedianFilter()
  	  :
  		  m_idx(0), m_cnt(0), m_med(0), isReady(false)
  {
  }

  /* addSample(s): adds the sample S to the window and computes the median
   * if enough samples have been gathered
   */
  void addSample(float s)
  {
    m_buf[m_idx] = s;
    m_idx = (m_idx + 1) % N;

    if(m_cnt == N){
      p_calcMedian();
    } else {
      m_cnt++;
      addSample(s);
    }
  }

  float Process(float Value)
  {
	  addSample(Value);
	  return isReady ? m_med : 0.;
  }


private:
  int m_idx, m_cnt;
  float m_buf[N], m_tmp[N], m_med;
  bool isReady;

  /* p_calcMedian(): helper to calculate the median. Copies
   * the buffer into the temp area, then calls Hoare's in-place
   * selection algorithm to obtain the median.
   */
  void p_calcMedian()
  {
    for(int i = 0; i < N; i++){
      m_tmp[i] = m_buf[i];
    }
    m_med = p_select(0, N - 1, N / 2);
    isReady = true;
  }

  /* p_partition(l, r, p): partition function, like from quicksort.
   * l and r are the left and right bounds of the array (m_tmp),
   * respectively, and p is the pivot index.
   */
  int p_partition(int l, int r, int p)
  {
    float tmp;
    float pv = m_tmp[p];
    m_tmp[p] = m_tmp[r];
    m_tmp[r] = pv;
    int s = l;
    for(int i = l; i < r; i++){
      if(m_tmp[i] < pv){
        tmp = m_tmp[s];
        m_tmp[s] = m_tmp[i];
        m_tmp[i] = tmp;
        s++;
      }
    }
    tmp = m_tmp[s];
    m_tmp[s] = m_tmp[r];
    m_tmp[r] = tmp;
    return s;
  }

  /* p_select(l, r, k): Hoare's quickselect. l and r are the
   * array bounds, and k conveys that we want to return
   * the k-th value
   */
  float p_select(int l, int r, int k)
  {
    if(l == r){
      return m_tmp[l];
    }
    int p = (l + r) / 2;
    p = p_partition(l, r, p);
    if(p == k){
      return m_tmp[k];
    }
    else if(k < p){
      return p_select(l, p - 1, k);
    }
    else {
      return p_select(p + 1, r, k);
    }
  }

};
