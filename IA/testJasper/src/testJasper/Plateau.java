package testJasper;
import se.sics.jasper.*;
import java.util.*;

public class Plateau {
	
	ArrayList<ArrayList<Character>> plat;
	
	public Plateau(){
		plat = new ArrayList<ArrayList<Character>>();
		for(int i=0;i<9;i++){
			ArrayList<Character> tmp = new ArrayList<Character>();
			plat.add(tmp);
		}
	}
	
	public void add(int numCase, char couleur){
		if(plat.get(numCase-1).size()< 3){
			plat.get(numCase-1).add(couleur);
		}
		else{
			System.out.println("Probleme pas plus de 3 pion par case ,  pb case :" + numCase);
		}
	}
	
	public void remove(int numCase){
		if(plat.get(numCase-1).size()> 0){
			plat.get(numCase-1).remove(plat.get(numCase-1).size()-1);
		}
		else{
			System.out.println("Probleme il n'y a plus de pion dans cette case,  pb case :" + numCase);
		}
	}
	
	public String toString(){
		String res="";
		for(ArrayList<Character> cas : plat){
			for(int j=cas.size()-1;j>=0;j--){
	        	char c = cas.get(j);
	        	res += c + " ";
	        }
			res+="\n";
		}
			
		return res;
	}
	
	public SPTerm toTerme(SICStus sp){
		
		
		
		try{
			
			//sp = new SICStus(null,null);
			SPTerm plateau = new SPTerm(sp);
			SPTerm N = new SPTerm(sp,"Noir");
	        SPTerm B = new SPTerm(sp,"Blanc");
	        
	        
	        /*for(ArrayList<Character> ca : plat){
	        	
	        	SPTerm cas = new SPTerm(sp);
				for(Character c : ca){					
					if(c == 'N'){
						cas.consList(N, cas);
					}
					if(c == 'B'){
						cas.consList(B, cas);
					}
				}
				plateau.consList(cas,plateau);	
			}*/
	        
	        
	        for(int i=8;i>=0;i--){
	        	SPTerm cas = new SPTerm(sp);
	        	for(int j=0;j<plat.get(i).size();j++){
		        	char c = plat.get(i).get(j);
		        	if(c == 'N'){
						cas.consList(N, cas);
					}
					if(c == 'B'){
						cas.consList(B, cas);
					}
		        }
	        	plateau.consList(cas,plateau);	
	        }
	        
	        
	        return plateau;
		}
		catch ( Exception e ){
            e.printStackTrace();
        }
		
		return null;
	}

}