<?php

/**
 * Menu callbacks.
 */

/**
 * This version takes the gene "name", infers the species from the 5-letter prefix and delegates to the more fully specified version
 */
//FIXME: a dreadful maintenance hack. putting in for now to bridge the context viewer to use of this service, but not sure it has the needed genus species data
const TRIPAL_LINKOUT_SPECIES_LOOKUP = [
	'medtr' => ['Medicago','truncatula'],
	'lotja' => ['Lotus','japonicus'],
	'cicar' => ['Cicer','arietinum'],
	'glyma' => ['Glycine','max'],
	'phavu' => ['Phaseolus','vulgaris'],
	'cajca' => ['Cajanus','cajan'],
	'vigra' => ['Vigna','radiata'],
	'aradu' => ['Arachis','duranensis'],
	'araip' => ['Arachis','ipaensis'],
	'arath' => ['Arabidopsis','thaliana'],
	'ambtr' => ['Amborella','trichopoda'],
	'orysa' => ['Oryza','sativa'],
	'prupe' => ['Prunus','persica'],
	'solly' => ['Solanum','lycopersicum'],
	'vitvi' => ['Vitis','vinifera'],
	'zeama' => ['Zea','mays'],
];
function gene_linkout_json($gene) {
    $species_prefix = substr($gene, 0, 5);
  //  error_log("species prefix is " + $species_prefix);
    $gene = substr($gene, 6);
    $genus = TRIPAL_LINKOUT_SPECIES_LOOKUP[$species_prefix][0];
    $species = TRIPAL_LINKOUT_SPECIES_LOOKUP[$species_prefix][1];
    tripal_linkout_json($genus, $species, $gene, '');
}

function family_representative_linkout_json($family_representative) {
	//find the gene by use of the "family representative" (ie label used for the phylonode, 
	//typically a polypeptide name (we stole this trick from the phylotree module)
        $sql = <<<SQL
          SELECT f.name AS gene_name, f.uniquename AS gene_uniquename, o.genus AS genus, o.species AS species
          FROM chado.featureprop fp, chado.feature f, chado.organism o
          WHERE fp.type_id = (SELECT cvterm_id FROM chado.cvterm
                              WHERE NAME = 'family representative')
          AND fp.value = :family_representative
          AND fp.feature_id = f.feature_id
          AND f.organism_id = o.organism_id
SQL;
        $args = array(':family_representative' => $family_representative);
        $result = chado_query($sql, $args);
        $fields = $result->fetchAssoc();
	full_gene_linkout_json($fields['genus'],$fields['species'],$fields['gene_name'],$fields['gene_uniquename'],$family_representative);
}

//simply preserve the signature that did not take the gene_uniquename for now
function tripal_linkout_json($genus,$species,$gene_name,$transcript_name) {
	full_gene_linkout_json($genus,$species,$gene_name,'',$transcript_name);
}

function full_gene_linkout_json($genus,$species,$gene_name,$gene_uniquename,$transcript_name) {
    $link =  [];
    $link[] = array(
            'href' => '/feature/'.$genus.'/'.$species.'/gene/'.$gene_uniquename,
            'text' => 'View LIS gene page for : ' . $gene_name,
        );

    if ($genus == 'Glycine' && $species == 'max') {

        $link[] = array(
            'href' => '/lis_context_viewer/index.html#/search/glyma.' . $gene_name . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene_name,
        );

        $link[] = array(
            'href' => 'http://www.soybase.org/sbt/search/search_results.php?category=FeatureName&search_term=' . $gene_name,
            'text' => 'view at SoyBase: ' . $gene_name,
        );
        
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4433&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );

        $link[] = array(
            'href' => 'http://soykb.org/gene_card.php?gene=' . $gene_name,
            'text' => 'view at SoyKB: ' . $gene_name,
        );

/*
        $link[] = array(
            'href' => 'http://www.genomicus.biologie.ens.fr/genomicus-plants/cgi-bin/search.pl?view=default&amp;query=' . $gene_name,
            'text' => 'view at Genomicus: ' . $gene_name,
        );
*/
    }
    else if ($genus == 'Phaseolus' && $species == 'vulgaris') {
        $link[] = array(
            'href' => '/lis_context_viewer/index.html#/search/phavu.' . $gene_name . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=3253&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
        $link[] = array(
            'href' => '/gbrowse_phavu1.0?query=name=' . $gene_name,
            'text' => 'view at LIS Pv1.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Lotus' && $species == 'japonicus') {
        $link[] = array(
            'href' => '/lis_context_viewer/index.html#/search/lotja.' . $gene_name . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene_name,
        );
        $link[] = array(
            'href' => '/gbrowse_lotja3.0?query=name=' . $gene_name,
            'text' => 'view at LIS Lj3.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Cicer' && $species == 'arietinum') {
        #hack: from the trees, the _gene is missing, but from the context viewer it is present
	$gene_name = preg_replace('/_gene$/','',$gene_name);
        $link[] = array(
            'href' => '/lis_context_viewer/index.html#/search/cicar.' . $gene_name . '_gene/',
            'text' => 'Find similar genomic contexts for : ' . $gene_name,
        );
        $link[] = array(
            'href' => '/gbrowse_cicar1.0?query=name=' . $gene_name,
            'text' => 'view at LIS Ca1.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Cajanus' && $species == 'cajan') {
        #hack: from the trees, the _gene is missing, but from the context viewer it is present
	$gene_name = preg_replace('/_gene$/','',$gene_name);
        $link[] = array(
            'href' => '/lis_context_viewer/index.html#/search/cajca.' . $gene_name . '_gene/',
            'text' => 'Find similar genomic contexts for : ' . $gene_name,
        );
        $link[] = array(
            'href' => '/gbrowse_cajca1.0?query=name=' . $gene_name,
            'text' => 'view at LIS Cc1.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Vigna' && $species == 'radiata') {
        $link[] = array(
            'href' => '/lis_context_viewer/index.html#/search/vigra.' . $gene_name ,
            'text' => 'Find similar genomic contexts for : ' . $gene_name,
        );
        $link[] = array(
            'href' => '/gbrowse_vigra1.0?query=name=' . $gene_name,
            'text' => 'view at LIS Vr1.0 GBrowse: ' . $gene_name,
        );
    }
    else if ($genus == 'Medicago' && $species == 'truncatula') {
        $link[] = array(
            'href' => '/lis_context_viewer/index.html#/search/medtr.' . $gene_name . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://medicago.jcvi.org/medicago/jbrowse/?data=data%2Fjson%2Fmedicago&loc='.$transcript_name.'&tracks=DNA%2Cgene_models&highlight=',
            'text' => 'view at JCVI: ' . $transcript_name,
        );
        $link[] = array(
            'href' => '/gbrowse_medtr4.0?query=name=' . $gene_name,
            'text' => 'view at LIS Mt4.0 GBrowse: ' . $gene_name,
        );
/*
        $link[] = array(
            'href' => 'http://medtr.comparative-legumes.org/gb2/gbrowse/Mt3.5.1?name=' . $gene_name,
            'text' => 'view at JLIS Mt3.5.1 GBrowse: ' . $gene_name,
        );
*/
        $link[] = array(
            'href' => 'http://www.medicagohapmap.org/fgb2/gbrowse/mt35/?name=' . $gene_name,
            'text' => 'view at Mt HapMap: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4432&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
/*
        $link[] = array(
            'href' => 'http://plantgrn.noble.org/LegumeIP/getseq.do?dataset_id=4&seq_acc=IMGA|' . $transcript_name,
            'text' => 'view at LegumeIP: ' . $transcript_name,
        );
*/
        $link[] = array(
            'href' => 'http://www.ncbi.nlm.nih.gov/gene/?term=' . $gene_name,
            'text' => 'view at NCBI Gene: ' . $gene_name,
        );
/*
        $link[] = array(
            'href' => 'http://www.genomicus.biologie.ens.fr/genomicus-plants/cgi-bin/search.pl?view=default&amp;query=MTR_' . str_replace("Medtr","", $gene_name),
            'text' => 'view at Genomicus: ' . $gene_name,
        );
*/
    }
    else if ($genus == 'Arachis' && $species == 'duranensis') {
        $link[] = array(
            'href' => '/lis_context_viewer/index.html#/search/aradu.' . $gene_name . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://peanutbase.org/gbrowse_aradu1.0?query=name=' . $gene_name . ';dbid=gene_models',
            'text' => 'view at PeanutBase: ' . $gene_name,
        );            
    }
    else if ($genus == 'Arachis' && $species == 'ipaensis') {
        $link[] = array(
            'href' => '/lis_context_viewer/index.html#/search/araip.' . $gene_name . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://peanutbase.org/gbrowse_araip1.0?query=name=' . $gene_name . ';dbid=gene_models',
            'text' => 'view at PeanutBase: ' . $gene_name,
        );        
    }
    else if ($genus == 'Arabidopsis' && $species == 'thaliana') {
        $link[] = array(
            'href' => 'http://www.araport.org/locus/' . $gene_name,
            'text' => 'view at Arabidopsis Information Portal: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://www.arabidopsis.org/servlets/TairObject?type=locus&name=' . $gene_name,
            'text' => 'view at TAIR: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=2296&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Zea' && $species == 'mays') {
	$gene_name = preg_replace('/_P\d+$/','',$gene_name);
        $link[] = array(
            'href' => 'http://maizegdb.org/gene_center/gene?id=' . $gene_name,
            'text' => 'view at MaizeGDB: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://www.gramene.org/Zea_mays/Gene/Summary?g=' . $gene_name,
            'text' => 'view at Gramene: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4431&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Oryza' && $species == 'sativa') {
        $link[] = array(
            'href' => 'http://rice.plantbiology.msu.edu/cgi-bin/gbrowse/rice/?name=' . $gene_name,
            'text' => 'view at MSU: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=3301&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Vitis' && $species == 'vinifera') {
        $link[] = array(
            'href' => 'http://www.genoscope.cns.fr/cgi-bin/ggb/vitis/12X/gbrowse/vitis/?name=' . $gene_name,
            'text' => 'view at Genoscope: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=2299&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Solanum' && $species == 'lycopersicum') {
        $link[] = array(
            'href' => 'http://solgenomics.net/jbrowse/current/?data=data%2Fjson%2FSL2.50&loc=' . $gene_name . '&tracks=DNA%2Cgene_models&highlight=',
            'text' => 'view at Sol Genomics Network: ' . $gene_name,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=3308&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Prunus' && $species == 'persica') {
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=2295&searchText=' . $gene_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }
    else if ($genus == 'Amborella' && $species == 'trichopoda') {
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4851&searchText=' . $transcript_name,
            'text' => 'view at Phytozome: ' . $gene_name,
        );
    }

    drupal_json_output($link);  
    drupal_exit();
}


?>
