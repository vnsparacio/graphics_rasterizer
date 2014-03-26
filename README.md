graphics_rasterizer
===================

A pipelined graphics rasterizer built in verilog
Project Results
Number of Rasterization Units:				________3x__________ 
(Right now we’re getting about 13 cycle/poly @ .4ns = 5.2ns/poly; to get to 2ns/poly, we’d need 3
Actual Clock Used (ns):			               	_________.4ns________



Design Optimizations:
1. Backface Culling
2. Bubble Smashing
3. Lower Precision
4. Multitest
5. Modified FSM (Attempted thoroughly but were ultimately unsuccessful)
 
1)	Backface culling
We knew backface culling would be a worthwhile investment, as it has several inherent benefits. The first of these benefits lies in its functionality, which simply reduces the total number of polygons rendered by eliminating the ones that are facing away from the camera; fewer polygons means faster performance.  By implementing culling, we also simplify the arithmetic of the rendering operation. Specifically, without culling, the triangle minterm operation contains 3 negations, 4 AND operations, and 1 OR operation. On the other hand, the culling implementation of the triangle minterms only contains 3 AND operations. This equates to increased performance while minimizing cost.
Implementing backface culling was rather straight forward, and we did not encounter any particularly difficult bugs. The most difficult part was figuring out which sides of the triangles to subtract and which variables corresponded to the vertices we were subtracting.
2)	Bubble Smashing
We chose to implement bubble smashing because of its minimal code additions and performance enhancements. We were able to create the bubble smashing parameter by merging halt_RnnnnL, which acts as an enable to receive new polygons and valid polygon. By doing this, we are able to eliminate one of the possible cases for halting, and keep the test iterator accepting polygons, thus saving clock cycles. We were aware that by adding this functionality, we would be increasing the total chip area, however we also knew that when backface culling is implemented, it can lead to a large amount of bubbles, which means a lot of wasted clock cycles. Since we are not as area constrained for this assignment as we were in previous assignments, we chose to implement the feature.

The bubble smash implementation was really not much more than altering the enable signal to not include invalid polygons as deemed by a set of edge equations. However, a more interesting part was updating the verification code to accommodate the new bubble smashing implementation. 

3)	Lower Precision
Lowering the precision of our multiplications, specifically in the sample test module, was a generally simple task. For this, we reduced the number of significant figures used initially based on the number bits required for super-sampling, scaling up until we could notice a difference in the diff’d output. We also just shrunk the number of bits that the input vectors were, which equated to smaller output vectors, which is huge in multiplication.

The challenging part was figuring out how much precision you could chop off without changing the output. We overcame this by looking at the radix and iteratively attempted to find what the optimal number of bits per vector was.

4)	Multitest

Multitest seemed like one of the most straightforward ways to obtain a value close to the design constraint of 2ns/poly. Even though the duplication of certain instantiated modules (such as two sample_tests and two hash trees) would significantly increase the area for a single chip, in the long run it actually ends up taking less space when multiple instances of the design run in parallel to reach the target time of 2ns/poly. For example, by testing two samples at once, we almost double the throughput, from 21 cycle/poly to 13 cycle/poly. With a .3-.4ns clock period, this equates to only have to use TWO copies of our design to reach the desired 2ns/poly. This is more desirable than having a .2 um smaller chip and having to run 5 more copies in parallel to reach the design specifications.
We implemented this optimization by creating a second sample, and calling two instances of sample test and the hash tree. Functionally, we maintained the same bounding box, but started with two samples: one in the lower left corner, and a second sample right above the first. These two sample contain a shared X coordinate and differ only in the y coordinate. When the right edge of the bounding box is reached, each of the sample locations jumps up two spaces, and their x coordinate reinitializes to the far left. This repeats until the end of the box is reached. 
		
   		
		
 		
Step 1: two samples begin in the lower left hand corner of the bounding box
		
		
		
		
Step 2: the samples begin to traverse the bounding box
		
		
		
		
Step 3: The right edge of the bounding box is reached, denoting the use of next_up_sample
		
		
		
		
Step 4: The sample location resets its X value and increments the Y value by two for both “testers.” This process will repeat until the end of the box has been reached.
The implementation of the multitest was particularly challenging, as it required that we keep track of a similar, yet different signal. We weighed the option of implementing the duplicated signal within the modules themselves vs simply instantiating two instances of certain modules. We chose the latter for simplification and easier expansion. The hardest part was being able to keep track of BOTH of the samplers while trying to minimize the number of adders needed to increment their X and Y coordinates. For example, instead of calculating BOTH a next_up_sample and a twice_up_sample, we utilized the FSM in test iterator to choose between calculating EITHER next_up_sample OR twice_up_sample. This way, we only use one addition per cycle, rather than calculating both each time. It was also important to recognize that the second “tester” would go out of the bounding box if there were an odd number of rows. This is fine in our design because the tester will merely label these samples as invalid, however, in future iterations for cleanliness, a feature could be implemented to NOT use the second tester when the top of the bounding box is reached.
5)	New FSM
The most disappointing of our unsuccessful endeavors includes trying to implement a new traversal algorithm. This particular algorithm was very similar to a “winding” search. The sample would move right on the triangle until it became invalid, it would then move up a row and continue right until another invalid sample was received, at this point it would reverse direction and jump backward to avoid overlapping signals, finally the sampler would continue in its new direction until another invalid sample was found. This was a new FSM we created to monitor the direction and movement of a sampler in a winding algorithm with the hopes of increasing our number of hits greatly. Although we were able to successfully implement the algorithm in separately coded Verilog, we were unable to successfully port this design to the Verilog of rasterizer. This was largely in part to the fact that the FSM was reliant on hit signals that came from the sample tester which dictated the direction the sampler was travelling. We spent hours trying to fix timing issues, however, latency of the “HIT” signal coming from sample test meant that the direction changes of our winding algorithm did not occur at the correct points. One solution to this would be to copy part of the sample test module and include it in the test iterator module, however, due to time constraints and simplicity, we chose not to proceed any farther with this optimization.
Analysis and Conclusion
This project, although frustrating, gave us great insight into the world of trade-offs. It was extremely interesting, for example, to see how changing the depth of a pipeline could positively affect timing while killing chip area, and vice versa. Not only this but, our successes and failures gave us an appreciation of the complexity of combining combinational and sequential logic. This gives rise to the need for careful planning, drawing of FSMs, and wave diagrams. We also gained insight into discerning positive optimizations vs dead end optimizations. We tried to implement quadrilaterals, however, came to realize that it did not make sense from a code readability aspect to use these over triangles, e.g. there are no “special cases” needed for triangles whereas for quads concavity must be considered, etc. 
One complaint we had was with the quality of the starter code/comments. There were a lot of inconsistencies with variable names and functions. The most annoying example was the HALT signal… which actually isn’t a halt at all rather than an enable. Overall, the project was a great learning experience and we truly feel that we’ve conquered a large obstacle when understanding both system Verilog and digital design.   


