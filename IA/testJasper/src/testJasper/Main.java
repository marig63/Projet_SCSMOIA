package preojet;

import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Scanner;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import se.sics.jasper.*;

public class Main {
	
	public static final byte[] intToByteArray(int value) {
	    return new byte[] {
	            (byte)(value >>> 24),
	            (byte)(value >>> 16),
	            (byte)(value >>> 8),
	            (byte)value};
	}
	
	public static int conversionEndian(byte[] t,int ordre){
		ByteBuffer buf;
		buf = ByteBuffer.wrap(t);
		if(ordre == 1){
			buf.order(ByteOrder.LITTLE_ENDIAN);
		}else{
			buf.order(ByteOrder.BIG_ENDIAN);
		}
		return buf.getInt();
	}
	
	public static Coup appelIA(SICStus sp,Plateau p,char coulP,int nbCoup){
		Coup coup = null;
		
        SPQuery query = null;
       
        try{
        	//-Djava.library.path="C:/Program Files/SICStus Prolog VC14 4.3.5/bin;%PATH%"
        	//listeCoupPossible([[],[N,N,N],[],[N,N],[],[N,N],[],[],[N,N,N]],_,0,Res).
        	//listeCoupPossible(Plateau,CouleurPion,NbPion,Res):-
        	

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
            	//System.out.println(res.toString());
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
	
    public static void main(String argv[]) throws SPException {
    	char coulJ ;
    	SICStus sp;
    	sp = new SICStus(null,null);
    	char coulA ;
    	int nbCoup = 0;
    	Plateau p = new Plateau();
        /*p.add(1, 'B');p.add(1, 'B');p.add(1, 'N');
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
        System.out.println(coup.toString());*/
    	
    	Scanner sc = new Scanner(System.in);
		System.out.println("Saisir port");
		int port = sc.nextInt();
	 
		System.out.println("Num port"+port);
		
		ServerSocket srv ;
		int caseDepAdv=1;
		int caseArrAdv=0;
		int caseDepCoup=0;
		int caseArrCoup=0;
		int couleur;
		int nbPartie = 0;
		int continu = 1;
		
		//ByteBuffer buf;
		byte[] byteEnvoie = new byte[4]; 
		byte[] byteRecu = new byte[4]; 
		try {
		    srv = new ServerSocket(port) ;
		    Socket s = srv.accept() ;
		    InputStream is = s.getInputStream();
		    OutputStream os = s.getOutputStream();
		    
		    couleur = is.read(byteRecu);
		    couleur = conversionEndian(byteRecu,1);
		    
		    if(couleur == 2){
		    	System.out.println("Vous posseder les pion noir "+couleur);
		    	caseDepCoup = 22;
		    	caseArrCoup = 22;
		    	coulJ = 'N' ;
		    	coulA = 'B';
		    }else{
		    	System.out.println("Vous posseder les pion blanc "+couleur);
		    	caseDepCoup = 44;
		    	caseArrCoup = 44;
		    	coulJ = 'B' ;
		    	coulA = 'N';
		    }
		    
		   while(nbPartie < 2){
		    	if((couleur == 2 && nbPartie == 0) || (couleur == 1 && nbPartie == 1)){
		    		System.out.println("Debut recption coup d'adverse");
		    		is = s.getInputStream();
		    		caseDepAdv = is.read(byteRecu);
		    		caseDepAdv = conversionEndian(byteRecu,1);
		    		
		    		caseArrAdv = caseDepAdv / 10;
		    		caseDepAdv = caseDepAdv%10;
		    		
		    		if(caseDepAdv != 0 && caseArrAdv != 0){
			    		if(caseArrAdv == caseDepAdv){
			    			p.add(caseDepAdv, coulA);
			    		}
			    		else{
			    			p.remove(caseDepAdv);
			    			p.add(caseArrCoup, coulA);
			    		}
		    		}
		    		System.out.println(p.toString());
		    		
		    		System.out.println("L'adversaire a jouer en "+caseDepAdv+"-"+caseArrAdv);
		    	}
		    	
		    	do{
		    			
		    		/* Appel Prolog */
		    		
		    		Coup coup = appelIA(sp,p,coulJ,nbCoup);
		            System.out.println(coup.toString());
		    		
		            
		            if( coup.depart != 0 && coup.arrivee != 0){
			    		if( coup.depart == coup.arrivee){
			    			p.add(coup.depart, coulJ);
			    		}
			    		else{
			    			p.remove(coup.depart);
			    			p.add(coup.arrivee, coulJ);
			    		}
		    		}
		            System.out.println(p.toString());
		            
		    		nbCoup++;
		    		
		    		caseDepCoup = coup.depart;
			    	caseArrCoup = coup.arrivee;
			    	
		    		/* */
		    		
		    		
		    		int envoi = caseArrCoup * 10 + caseDepCoup;
		    		os.write(conversionEndian(intToByteArray(envoi),0));
		    		os.flush();
		    		System.out.println("J'ai envoie1: "+conversionEndian(intToByteArray(caseDepCoup),0)+"-"+conversionEndian(intToByteArray(caseArrCoup),0));
		    		    
		    		is = s.getInputStream();
		    		    
		    		caseDepAdv = is.read(byteRecu);
		    		caseDepAdv = conversionEndian(byteRecu,1);
			    		
		    		caseArrAdv = caseDepAdv / 10;
		    		caseDepAdv = caseDepAdv%10;
		    		System.out.println("L'adversaire a jouer en "+caseDepAdv+"-"+caseArrAdv);
		    		/*  */
		    		
		    		if(caseDepAdv != 0 && caseArrAdv != 0){
			    		if(caseArrAdv == caseDepAdv){
			    			p.add(caseDepAdv, coulA);
			    		}
			    		else{
			    			p.remove(caseDepAdv);
			    			p.add(caseArrCoup, coulA);
			    		}
		    		}
		    		System.out.println(p.toString());

		    		/*   */
			    		
		    		continu = is.read(byteRecu);
		    		continu = conversionEndian(byteRecu,1);
		    		if(continu == 1){
		    			 System.out.println("La partie continu");
		    		}
		    		else{
		    			 System.out.println("La partie est terminer");
		    		}
		    		 
		    	}
		    	while(continu == 1);
		    		nbPartie++;
		    		p = new Plateau();
		    		nbCoup = 0;
		    	}
		    		
		    		
	
		    is.close();
		    s.close();
		    os.close();
		    srv.close();
		} catch(IOException e) { }
        
    }
}