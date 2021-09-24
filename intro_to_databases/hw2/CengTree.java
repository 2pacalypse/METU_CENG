import java.util.ArrayList;

public class CengTree
{
	public CengTreeNode root;
	
	public CengTree(Integer order)
	{
		CengTreeNode.order = order;
		
		// TODO: Create an empty Tree
		root =  new CengTreeNodeLeaf(null);
		
		
	}
	
	public void addGift(CengGift gift)
	{
		int maxSize = 2 * CengTreeNode.order;

		// TODO: Insert Gift to Tree
		if(root.type == CengNodeType.Leaf){
			CengTreeNodeLeaf r = (CengTreeNodeLeaf) root;
			if(r.giftCount() < maxSize){
				r.insertToLeaf(gift);
			}else{
				CengTreeNodeInternal newroot;
				newroot = new CengTreeNodeInternal(null);
				
				r.insertToLeaf(gift);
				int slicePos = CengTreeNode.order;
				
				ArrayList<CengGift> c1array = new ArrayList<CengGift> (r.getGifts().subList(0, slicePos));
				ArrayList<CengGift> c2array = new ArrayList<CengGift> (r.getGifts().subList(slicePos, r.giftCount()));
				
				CengTreeNodeLeaf child1 = new CengTreeNodeLeaf(newroot, c1array);
				CengTreeNodeLeaf child2 = new CengTreeNodeLeaf(newroot, c2array);
				
				newroot.getAllChildren().add(child1);
				newroot.getAllChildren().add(child2);
				newroot.getKeys().add(child2.giftKeyAtIndex(0));
				root = newroot;
			}
		}else{
			CengTreeNode temp =  root;			
			int giftKey = gift.key();
			while(temp.type == CengNodeType.Internal){

				CengTreeNodeInternal temp2 = (CengTreeNodeInternal) temp;

				/*int i = 0;
				while(i < temp2.getKeys().size() && giftKey > temp2.keyAtIndex(i) && i < CengTreeNode.order*2 + 1 ) i++;
				*/
				int l = 0;
				int h = temp2.getKeys().size()-1;
				while(l <= h){
					int m = (l+h)/2;
					if(giftKey < temp2.getKeys().get(m).intValue()){
						h = m-1;
					}else if(giftKey > temp2.getKeys().get(m).intValue()){
						l = m +1;
					}
				}
				
				temp = temp2.getAllChildren().get(l);
			}
			
			CengTreeNodeLeaf temp2 = (CengTreeNodeLeaf) temp;
			if(temp2.giftCount() < maxSize){
				temp2.insertToLeaf(gift);
			}else{
				CengTreeNodeInternal ptemp2 =  (CengTreeNodeInternal) temp2.getParent();
				
				
				int slicePos = CengTreeNode.order;				
				
				temp2.insertToLeaf(gift);

				
				ArrayList<CengGift> c1array = new ArrayList<CengGift> (temp2.getGifts().subList(0, slicePos));
				ArrayList<CengGift> c2array = new ArrayList<CengGift> (temp2.getGifts().subList(slicePos, temp2.giftCount()));
				
				CengTreeNodeLeaf child1 = new CengTreeNodeLeaf(ptemp2, c1array);
				CengTreeNodeLeaf child2 = new CengTreeNodeLeaf(ptemp2, c2array);
	
				if(ptemp2.getKeys().size() < maxSize){

					int changedPos = ptemp2.insertToInternal(child2.giftKeyAtIndex(0));
					ptemp2.getAllChildren().remove(changedPos);
					ptemp2.getAllChildren().add(changedPos, child1);
					ptemp2.getAllChildren().add(changedPos + 1, child2);

				}else{
					
					int changedPos = ptemp2.insertToInternal(child2.giftKeyAtIndex(0));
					ptemp2.getAllChildren().remove(changedPos);
					ptemp2.getAllChildren().add(changedPos, child1);
					ptemp2.getAllChildren().add(changedPos + 1, child2);
					pushFromOverflow(ptemp2);
				}
				
				
			}
			
		}
		
		
	}
	
	
	
	public ArrayList<CengTreeNode> searchGift(Integer key)
	{
		// TODO: Search within whole Tree, return visited nodes.
		// Return null if not found.
		CengTreeNode temp = root;
		while(temp.type == CengNodeType.Internal){
			CengTreeNodeInternal temp2 = (CengTreeNodeInternal) temp;
			/*int i = 0;
			while(i < temp2.getKeys().size() && key >= temp2.keyAtIndex(i) && i < CengTreeNode.order*2 + 1 ) i++;
			*/
			int l = 0;
			int h = temp2.getKeys().size()-1;
			while(l <= h){
				int m = (l+h)/2;
				if(key.intValue() < temp2.getKeys().get(m).intValue()){
					h = m-1;
				}else if(key.intValue() >= temp2.getKeys().get(m).intValue()){
					l = m +1;
				}
			}
			
			temp = temp2.getAllChildren().get(l);
		}
	
		CengTreeNodeLeaf temp2 = (CengTreeNodeLeaf) temp;
		
		/*int i = 0;
		while(i < temp2.getGifts().size() && key > temp2.giftKeyAtIndex(i) && i < CengTreeNode.order*2 + 1 ) i++;
		*/
		int l = 0;
		int h = temp2.getGifts().size()-1;
		while(l <= h){
			int m = (l+h)/2;
			if(key.intValue() < temp2.giftKeyAtIndex(m).intValue()){
				h = m-1;
			}else if(key.intValue() > temp2.giftKeyAtIndex(m).intValue()){
				l = m +1;
			}else{
				l = m;
				break;
			}
		}
		
		if(l < temp2.giftCount() && temp2.getGifts().get(l).key().equals(key)){
			ArrayList<CengTreeNode> x = new ArrayList<CengTreeNode>();
			x.add(temp2);
			CengTreeNode temp3 = temp2.getParent();
			StringBuilder s = new StringBuilder();
			
			while(temp3 != null){
				x.add(0,temp3);
				 temp3 =  temp3.getParent();
			}
			
			for(int m = 0; m < x.size() -1; m++){
				CengTreeNodeInternal y = (CengTreeNodeInternal) x.get(m);
				if(m > 0)
					s.append('\t');
				
				System.out.println(s.toString() + ("<index>"));
				for(int k = 0; k < y.getKeys().size();k++){
					System.out.println(s.toString() + (y.getKeys().get(k)));
				}
				System.out.println(s.toString() + ("</index>"));
				}


			
			if(x.size() > 1)s.append('\t');
			//System.out.println(s.toString()  + "<data>");
		
			System.out.print(s.toString() + "<record>");
			System.out.println(temp2.getGifts().get(l).key().toString() + '|' + temp2.getGifts().get(l).name()+ '|' + temp2.getGifts().get(l).material() + '|' + temp2.getGifts().get(l).color() + "</record>");
			//System.out.println(s.toString()  + "</data>");
			return x;
		}
		System.out.println("Could not find " + key.toString() + '.');
		return null;
	}
	
	public void printTree()
	{
		// TODO: Print the whole tree to console
		printHelper(root, 0);
	}
	
	public void printHelper(CengTreeNode x, int tabSize){
		StringBuilder s = new StringBuilder();
		for(int j = 0; j < tabSize;j++) {s.append('\t');}
		if(x.type == CengNodeType.Leaf){
			
			CengTreeNodeLeaf y = (CengTreeNodeLeaf) x;
			System.out.println(s.toString()  + "<data>");
			for(int l = 0; l < y.giftCount(); l++){
				System.out.print(s.toString() + "<record>");
				System.out.println(y.getGifts().get(l).key().toString() + '|' + y.getGifts().get(l).name()+ '|' + y.getGifts().get(l).material() + '|' + y.getGifts().get(l).color() + "</record>");
			}
			System.out.println(s.toString()  + "</data>");
		}else{
			CengTreeNodeInternal y = (CengTreeNodeInternal) x;
			System.out.println(s.toString() + ("<index>"));
			for(int k = 0; k < y.getKeys().size();k++){
				System.out.println(s.toString() + (y.getKeys().get(k)));
			}
			System.out.println(s.toString() + ("</index>"));
			for(int m = 0; m < y.getAllChildren().size();m++){
				printHelper(y.getAllChildren().get(m), tabSize + 1);
			}
		}
		
			
		}
	
	
	// Extra Functions
	
	public void pushFromOverflow(CengTreeNodeInternal ptemp2){
		int slicePos = CengTreeNode.order;
		int maxSize = 2*CengTreeNode.order;
		ArrayList<Integer> keys1 = new ArrayList <Integer> (ptemp2.getKeys().subList(0, slicePos));
		ArrayList<Integer> keys2 = new ArrayList <Integer> (ptemp2.getKeys().subList(slicePos, ptemp2.getKeys().size()));
		
		ArrayList<CengTreeNode> ch1 = new ArrayList <CengTreeNode> (ptemp2.getAllChildren().subList(0, slicePos + 1));
		ArrayList<CengTreeNode> ch2 = new ArrayList <CengTreeNode> (ptemp2.getAllChildren().subList(slicePos + 1, ptemp2.getAllChildren().size()));
		
		int pushedVal = keys2.get(0);
		keys2.remove(0);
		if(ptemp2 == root){
			CengTreeNodeInternal newroot = new CengTreeNodeInternal(null);
			
			
			CengTreeNodeInternal x = new CengTreeNodeInternal(newroot, keys1, ch1);
			for(int i=0; i < ch1.size();i++){
				ch1.get(i).setParent(x);
			}
			CengTreeNodeInternal y = new CengTreeNodeInternal(newroot, keys2, ch2);
			for(int i=0; i < ch2.size();i++){
				ch2.get(i).setParent(y);
			}
			newroot.getKeys().add(pushedVal);
			newroot.getAllChildren().add(x);
			newroot.getAllChildren().add(y);
			root = newroot;
		}else{
			CengTreeNodeInternal par = (CengTreeNodeInternal) ptemp2.getParent();
			
			CengTreeNodeInternal x = new CengTreeNodeInternal(par, keys1, ch1);
			for(int i=0; i < ch1.size();i++){
				ch1.get(i).setParent(x);
			}
			CengTreeNodeInternal y = new CengTreeNodeInternal(par, keys2, ch2);
			for(int i=0; i < ch2.size();i++){
				ch2.get(i).setParent(y);
			}
			
			int changedPos = par.insertToInternal(pushedVal);
			par.getAllChildren().remove(changedPos);
			par.getAllChildren().add(changedPos, x);
			par.getAllChildren().add(changedPos + 1, y);
			
			if(par.getKeys().size() > maxSize){
				pushFromOverflow(par);
				
			}
			
		}
	}
}
