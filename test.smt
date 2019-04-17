(declare-const y Int)
(declare-const x Int)

(assert (not (=


(or false (and (> (+ (+ 6)) 5) (> (+ (- (+ 6)) (* 1 y)) 0) (= (mod (+ 6) 1) 0)))

;dovrebbe essere equivalente a x > 5 and x < y

(> y 6)


)))

(check-sat) ;stampa unsat se le formule sono equivalenti