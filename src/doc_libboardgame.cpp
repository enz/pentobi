/**

@page libboardgame_doc_tags Tags used in documentation
This page defines attributes of documentation elements that are in
widespread use. For brevity, the documentation block contains only
a reference to the section of this page.

@section libboardgame_avoid_stack_allocation Class size is large
The size of this class is large because it contains large members that are not
allocated on the heap to avoid dereferencing pointers for speed reasons. It
should be avoided to create instances of this class on the stack.

@section libboardgame_doc_obj_ref_opt Object reference optimization
This class uses a reference to a certain object several times but does not
store the reference at construction time for memory and/or speed optimization.
The reference is passed as an argument to the functions that need it. The
class instance assumes (and might check with assertions) that the reference
always refers to the same object .

@section libboardgame_doc_storesref Stores a reference
Used for parameters to indicate that the class will store a reference to the
parameter. The lifetime of the parameter must exceed the lifetime of the
constructed class.

@section libboardgame_doc_threadsafe_after_construction Thread-safe after
construction
Used for classes that, that are thread-safe (w.r.t. different instances) after
construction. The constructor (and potentially also the destructor) is not
thread-safe, for example because it modifies non-const static class members.


@page libboardgame_doc_glossary Glossary
This page explains and defines terms used in the documentation.

@section libboardgame_doc_gogui GoGui
Graphical interface for Go engines using GTP. Defines several GTP extension
commands. http://gogui.sf.net

@section libboardgame_doc_gnugo GNU Go
GNU Go program http://www.gnu.org/s/gnugo/

@section libboardgame_doc_gtp GTP
Go Text Protocol http://www.lysator.liu.se/~gunnar/gtp/

@section libboardgame_doc_uct UCT
Upper Confidence bounds applied to Tree: a Monte-Carlo tree search algorithm
that applies bandit ideas to the move selection at tree nodes.
See @ref libboardgame_doc_kocsis_szepesvari_2006

@section libboardgame_doc_rave RAVE
Rapid Action Value Estimation: Keeps track of the value of a move averaged
over all simulations in the subtree of a node in which the move was played
by a player in a position following the node (inclusive).
See @ref libboardgame_doc_gelly_silver_2007

@section libboardgame_doc_sgf SGF
Smart Game Format http://www.red-bean.com/sgf/

@page libboardgame_doc_bibliography Bibliography
List of publications.

@section libboardgame_doc_alphago_2016  Mastering the game of Go with deep neural networks and tree search.
D. Silver, A. Huang, et al. Nature 529 (7587), pp. 484-489, 2016.
<a href="https://storage.googleapis.com/deepmind-media/alphago/AlphaGoNaturePaper.pdf">(PDF)</a>

@section libboardgame_doc_enz_2009 A Lock-free Multithreaded Monte-Carlo Tree Search Algorithm.
M. Enzenberger, M. Mueller. Advances in Computer Games 2009.
<a href="http://webdocs.cs.ualberta.ca/~mmueller/ps/enzenberger-mueller-acg12.pdf">(PDF)</a>

@section libboardgame_doc_gelly_silver_2007 Combining Online and Offline Knowledge in UCT.
S. Gelly, D. Silver. Proceedings of the 24th international conference on Machine learning, pp. 273-280, 2007.
<a href="http://www.machinelearning.org/proceedings/icml2007/papers/387.pdf">(PDF)</a>

@section libboardgame_doc_kocsis_szepesvari_2006 Bandit Based Monte-Carlo Planning
L. Kocsis, Cs. Szepesvári. Proceedings of the 17th European Conference on
Machine Learning, Springer-Verlag, Berlin, LNCS/LNAI 4212, September 18-22,
pp. 282-293, 2006
<a href="http://www.sztaki.hu/~szcsaba/papers/ecml06.pdf">(PDF)</a>

*/
