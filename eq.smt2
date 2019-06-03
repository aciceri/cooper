(declare-const y Int)
(assert (not (= (or false (and (= (+ (+ (* -2 y) 1) (* 2 y)) 1) (= (mod (+ (* 1 1) (+ (* -2 y) 1)) 2) 0) (= (mod (+ (* -2 y) 1) 1) 0)) false (and (= (+ (+ (* -2 y) 2) (* 2 y)) 1) (= (mod (+ (* 1 1) (+ (* -2 y) 2)) 2) 0) (= (mod (+ (* -2 y) 2) 1) 0))) true)))
(check-sat)

