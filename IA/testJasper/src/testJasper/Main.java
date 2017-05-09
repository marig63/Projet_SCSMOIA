package preojet;
import se.sics.jasper.*;

public class Main {
	
	public static Coup appelIA(Plateau p,char coulP,int nbCoup){
		Coup coup = null;
		SICStus sp;
        SPQuery query = null;
       
        try{
        	//-Djava.library.path="C:/Program Files/SICStus Prolog VC14 4.3.5/bin;%PATH%"
        	//listeCoupPossible([[],[N,N,N],[],[N,N],[],[N,N],[],[],[N,N,N]],_,0,Res).
        	//listeCoupPossible(Plateau,CouleurPion,NbPion,Res):-
        	sp = new SICStus(null,null);

            //sp.load("C:/Users/guillaume/workspace/testJasper/src/testJasper/test.pl");
        	sp.load("/afs/deptinfo-st.univ-fcomte.fr/users/gmarisa/workspace/preojet/bin/preojet/test.pl");

            SPPredicate pred = new SPPredicate(sp, "listeCoupPossible", 4, "");
            
            SPTerm N = new SPTerm(sp,"N");
            SPTerm B = new SPTerm(sp,"B");
            
            SPTerm plateau = p.toTerme(sp);
            
            //System.out.println(p.toString());
            //System.out.println(plateau.toString());
            
            SPTerm nbPion = new SPTerm(sp, nbCoup);
            SPTerm res = new SPTerm(sp).putVariable();

            if(coulP == 'N'){
            	query = sp.openQuery(pred, new SPTerm[] { plateau, N, nbPion, res});
			}
			if(coulP == 'B'){
				query = sp.openQuery(pred, new SPTerm[] { plateau, B, nbPion, res});
			}
            //query = sp.openQuery(pred, new SPTerm[] { plateau, N, nbPion, res});

            SPTerm t[] = new SPTerm[2];
            String s1 = "" ;
            String s2 = "";
            while (query.nextSolution())
            {
            	try{
            	System.out.println(res.toString());
                t = res.toTermArray();
                s1 = t[0].toString();
                s2 = t[1].toString();
            	}
            	catch(Exception e){
            		System.out.print(e);            	}
            }  
            coup = new Coup(	Integer.parseInt(s1)	,	Integer.parseInt(s2)	);    
        }
        catch ( Exception e )
        {
            e.printStackTrace();
        }
		return coup;	
	}
	
    public static void main(String argv[]) {

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

    	
        Coup coup = appelIA(p,'B',8);
        System.out.println(coup.toString());
        
    }
}