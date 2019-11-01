if [ -z "$J4_PATH" ]
then
	echo "Error: J4_PATH environment variable is empty. Please set it to the location of J4"
else
	alias mall="(cd $J4_PATH; make clean; make)"
	alias mc="(cd $J4_PATH; make clean)"
	alias mall="(cd $J4_PATH; make clean; make)"
	alias mp="(cd $J4_PATH/parse; make clean; make)"
	alias mvm="(cd $J4_PATH/vm; make clean; make)"
	alias cmp="$J4_PATH/compile.sh"
	qkrn() {
		if [ ! -z "$1" ] ; then
			dummy=longnamedoesntmatter	
			cmp "$1" $dummy
			res=$?
			if [ $res ] ; then
				./$dummy
				rm $dummy
			fi
		else
			echo "qkrn: no argument"
		fi
	}
fi
