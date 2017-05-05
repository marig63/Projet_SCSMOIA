package testJasper;

import se.sics.jasper.*;

public class Main {
	

	
    public static void main(String argv[]) {

        SICStus sp;
        SPQuery query;
        try{
        	//-Djava.library.path="C:/Program Files/SICStus Prolog VC14 4.3.5/bin;%PATH%"
        	//listeCoupPossible([[],[N,N,N],[],[N,N],[],[N,N],[],[],[N,N,N]],_,0,Res).
        	//listeCoupPossible(Plateau,CouleurPion,NbPion,Res):-
        	sp = new SICStus(argv,null);

            sp.load("C:/Users/guillaume/workspace/testJasper/src/testJasper/test.pl");

            SPPredicate pred = new SPPredicate(sp, "listeCoupPossible", 4, "");
            //SPTerm plateau = new SPTerm(sp, "[[],[N,N,N],[],[N,N],[],[N,N],[],[],[N,N,N]]");
            
            SPTerm N = new SPTerm(sp,"Noir");
            SPTerm B = new SPTerm(sp,"Blanc");
            
            Plateau p = new Plateau();
            p.add(1, 'B');p.add(1, 'B');p.add(1, 'N');
            p.add(2, 'N');
            p.add(3, 'N');
            p.add(4, 'N');
            p.add(5, 'B');
            p.add(6, 'N');
            p.add(7, 'B');p.add(7, 'N');
            p.add(8, 'B');
            p.add(9, 'N'); p.add(9, 'B');p.add(9, 'B');
            p.remove(1);
            //p.remove(2); //p.remove(2);
            p.remove(9);
            
            
            SPTerm plateau = p.toTerme(sp);
            
            System.out.println(p.toString());
            System.out.println(plateau.toString());
            
           
            
            SPTerm couleurPion = new SPTerm(sp, "N");
            SPTerm nbPion = new SPTerm(sp, 0);
            SPTerm res = new SPTerm(sp).putVariable();

            query = sp.openQuery(pred, new SPTerm[] { plateau, couleurPion, nbPion, res});

            while (query.nextSolution())
            {
                System.out.println(res.toString());
            }
        }
        catch ( Exception e )
        {
            e.printStackTrace();
        }
    }
}
