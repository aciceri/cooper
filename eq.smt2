(declare-const k Int)
(assert (not (= (or false (and (= (+ (+ (* 2 k) 1) (* -2 k)) 1) (= (mod (+ (* 1 1) (+ (* 2 k) 1)) 2) 0) (= (mod (+ (* 2 k) 1) 1) 0)) false (and (= (+ (+ (* 2 k) 2) (* -2 k)) 1) (= (mod (+ (* 1 1) (+ (* 2 k) 2)) 2) 0) (= (mod (+ (* 2 k) 2) 1) 0))) true)))
(check-sat)

