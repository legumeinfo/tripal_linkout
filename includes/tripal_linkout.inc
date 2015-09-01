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
];
function gene_linkout_json($gene) {
    $species_prefix = substr($gene, 0, 5);
  //  error_log("species prefix is " + $species_prefix);
    $gene = substr($gene, 6);
    $genus = TRIPAL_LINKOUT_SPECIES_LOOKUP[$species_prefix][0];
    $species = TRIPAL_LINKOUT_SPECIES_LOOKUP[$species_prefix][1];
    tripal_linkout_json($genus, $species, $gene, '');
}

function tripal_linkout_json($genus,$species,$gene,$transcript) {
    $link =  [];

    if ($genus == 'Glycine' && $species == 'max') {
        $link[] = array(
            'href' => '/lis_gene_families/chado/context_viewer/search/glyma.' . $gene . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene,
        );

        $link[] = array(
            'href' => 'http://www.soybase.org/sbt/search/search_results.php?category=FeatureName&search_term=' . $gene,
            'text' => 'view at SoyBase: ' . $gene,
        );
        
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4433&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );

        $link[] = array(
            'href' => 'http://soykb.org/gene_card.php?gene=' . $gene,
            'text' => 'view at SoyKB: ' . $gene,
        );

/*
        $link[] = array(
            'href' => 'http://www.genomicus.biologie.ens.fr/genomicus-plants/cgi-bin/search.pl?view=default&amp;query=' . $gene,
            'text' => 'view at Genomicus: ' . $gene,
        );
*/
    }
    else if ($genus == 'Phaseolus' && $species == 'vulgaris') {
        $link[] = array(
            'href' => '/lis_gene_families/chado/context_viewer/search/phavu.' . $gene . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=3253&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );
        $link[] = array(
            'href' => '/gbrowse_phavu1.0?query=name=' . $gene,
            'text' => 'view at LIS Pv1.0 GBrowse: ' . $gene,
        );
    }
    else if ($genus == 'Lotus' && $species == 'japonicus') {
        $link[] = array(
            'href' => '/lis_gene_families/chado/context_viewer/search/lotja.' . $gene . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene,
        );
        $link[] = array(
            'href' => '/gbrowse_lotja3.0?query=name=' . $gene,
            'text' => 'view at LIS Lj3.0 GBrowse: ' . $gene,
        );
    }
    else if ($genus == 'Cicer' && $species == 'arietinum') {
        $link[] = array(
            'href' => '/lis_gene_families/chado/context_viewer/search/cicar.' . $gene . '_gene/',
            'text' => 'Find similar genomic contexts for : ' . $gene,
        );
        $link[] = array(
            'href' => '/gbrowse_cicar1.0?query=name=' . $gene,
            'text' => 'view at LIS Ca1.0 GBrowse: ' . $gene,
        );
    }
    else if ($genus == 'Cajanus' && $species == 'cajan') {
        $link[] = array(
            'href' => '/lis_gene_families/chado/context_viewer/search/cajca.' . $gene . '_gene/',
            'text' => 'Find similar genomic contexts for : ' . $gene,
        );
        $link[] = array(
            'href' => '/gbrowse_cajca1.0?query=name=' . $gene,
            'text' => 'view at LIS Cc1.0 GBrowse: ' . $gene,
        );
    }
    else if ($genus == 'Vigna' && $species == 'radiata') {
        $link[] = array(
            'href' => '/lis_gene_families/chado/context_viewer/search/vigra.' . $gene ,
            'text' => 'Find similar genomic contexts for : ' . $gene,
        );
        $link[] = array(
            'href' => '/gbrowse_vigra1.0?query=name=' . $gene,
            'text' => 'view at LIS Vr1.0 GBrowse: ' . $gene,
        );
    }
    else if ($genus == 'Medicago' && $species == 'truncatula') {
        $link[] = array(
            'href' => '/lis_gene_families/chado/context_viewer/search/medtr.' . $gene . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene,
        );
        $link[] = array(
            'href' => 'http://medicago.jcvi.org/medicago/jbrowse/?data=data%2Fjson%2Fmedicago&loc='.$transcript.'&tracks=DNA%2Cgene_models&highlight=',
            'text' => 'view at JCVI: ' . $transcript,
        );
        $link[] = array(
            'href' => '/gbrowse_medtr4.0?query=name=' . $gene,
            'text' => 'view at LIS Mt4.0 GBrowse: ' . $gene,
        );
/*
        $link[] = array(
            'href' => 'http://medtr.comparative-legumes.org/gb2/gbrowse/Mt3.5.1?name=' . $gene,
            'text' => 'view at JLIS Mt3.5.1 GBrowse: ' . $gene,
        );
*/
        $link[] = array(
            'href' => 'http://www.medicagohapmap.org/fgb2/gbrowse/mt35/?name=' . $gene,
            'text' => 'view at Mt HapMap: ' . $gene,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4432&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );
/*
        $link[] = array(
            'href' => 'http://plantgrn.noble.org/LegumeIP/getseq.do?dataset_id=4&seq_acc=IMGA|' . $transcript,
            'text' => 'view at LegumeIP: ' . $transcript,
        );
*/
        $link[] = array(
            'href' => 'http://www.ncbi.nlm.nih.gov/gene/?term=' . $gene,
            'text' => 'view at NCBI Gene: ' . $gene,
        );
/*
        $link[] = array(
            'href' => 'http://www.genomicus.biologie.ens.fr/genomicus-plants/cgi-bin/search.pl?view=default&amp;query=MTR_' . str_replace("Medtr","", $gene),
            'text' => 'view at Genomicus: ' . $gene,
        );
*/
    }
    else if ($genus == 'Arachis' && $species == 'duranensis') {
        $link[] = array(
            'href' => '/lis_gene_families/chado/context_viewer/search/aradu.' . $gene . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene,
        );
        $link[] = array(
            'href' => 'http://peanutbase.org/gbrowse_aradu1.0?query=name=' . $gene . ';dbid=gene_models',
            'text' => 'view at PeanutBase: ' . $gene,
        );            
    }
    else if ($genus == 'Arachis' && $species == 'ipaensis') {
        $link[] = array(
            'href' => '/lis_gene_families/chado/context_viewer/search/araip.' . $gene . '/',
            'text' => 'Find similar genomic contexts for : ' . $gene,
        );
        $link[] = array(
            'href' => 'http://peanutbase.org/gbrowse_araip1.0?query=name=' . $gene . ';dbid=gene_models',
            'text' => 'view at PeanutBase: ' . $gene,
        );        
    }
    else if ($genus == 'Arabidopsis' && $species == 'thaliana') {
        $link[] = array(
            'href' => 'http://www.araport.org/locus/' . $gene,
            'text' => 'view at Arabidopsis Information Portal: ' . $gene,
        );
        $link[] = array(
            'href' => 'http://www.arabidopsis.org/servlets/TairObject?type=locus&name=' . $gene,
            'text' => 'view at TAIR: ' . $gene,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=2296&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );
    }
    else if ($genus == 'Zea' && $species == 'mays') {
	$gene = preg_replace('/_P\d+$/','',$gene);
        $link[] = array(
            'href' => 'http://maizegdb.org/gene_center/gene?id=' . $gene,
            'text' => 'view at MaizeGDB: ' . $gene,
        );
        $link[] = array(
            'href' => 'http://www.gramene.org/Zea_mays/Gene/Summary?g=' . $gene,
            'text' => 'view at Gramene: ' . $gene,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4431&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );
    }
    else if ($genus == 'Oryza' && $species == 'sativa') {
        $link[] = array(
            'href' => 'http://rice.plantbiology.msu.edu/cgi-bin/gbrowse/rice/?name=' . $gene,
            'text' => 'view at MSU: ' . $gene,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=3301&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );
    }
    else if ($genus == 'Vitis' && $species == 'vinifera') {
        $link[] = array(
            'href' => 'http://www.genoscope.cns.fr/cgi-bin/ggb/vitis/12X/gbrowse/vitis/?name=' . $gene,
            'text' => 'view at Genoscope: ' . $gene,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=2299&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );
    }
    else if ($genus == 'Solanum' && $species == 'lycopersicum') {
        $link[] = array(
            'href' => 'http://solgenomics.net/jbrowse/current/?data=data%2Fjson%2FSL2.50&loc=' . $gene . '&tracks=DNA%2Cgene_models&highlight=',
            'text' => 'view at Sol Genomics Network: ' . $gene,
        );
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=3308&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );
    }
    else if ($genus == 'Prunus' && $species == 'persica') {
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=2295&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );
    }
    else if ($genus == 'Amborella' && $species == 'trichopoda') {
        $link[] = array(
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=4851&searchText=' . $transcript,
            'text' => 'view at Phytozome: ' . $gene,
        );
    }

    drupal_json_output($link);  
    drupal_exit();
}


?>
