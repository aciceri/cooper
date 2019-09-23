(declare-const x Int)
(assert (not (= (or false (and (= (+ (* -1 x) (+ (* 1 x) 0)) 0) (= (mod (+ (* -1 1) (* -1 x)) -2) 0) (= (mod (+ (* 1 x) 0) 2) 0)) false (and (= (+ (* -1 x) (+ (* 1 x) 1)) 0) (= (mod (+ (* -1 1) (* -1 x)) -2) 0) (= (mod (+ (* 1 x) 1) 2) 0))) true)))
(check-sat)

