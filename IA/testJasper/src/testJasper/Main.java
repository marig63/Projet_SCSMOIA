package testJasper;

import se.sics.jasper.*;

public class Main {
    public static void main(String argv[]) {

        SICStus sp;
        SPTerm from, to, way;
        SPQuery query;
        int i;

        try{
        	//listeCoupPossible([[],[N,N,N],[],[N,N],[],[N,N],[],[],[N,N,N]],_,0,Res).
        	//listeCoupPossible(Plateau,CouleurPion,NbPion,Res):-
        	sp = new SICStus(argv,null);

            sp.load("C:/Users/guillaume/workspace/testJasper/src/testJasper/test.pl");

            SPPredicate pred = new SPPredicate(sp, "listeCoupPossible", 4, "");
            SPTerm plateau = new SPTerm(sp, "[[],[N,N,N],[],[N,N],[],[N,N],[],[],[N,N,N]]");
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