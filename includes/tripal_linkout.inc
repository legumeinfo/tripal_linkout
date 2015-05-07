<?php

/**
 * Menu callbacks.
 */

function tripal_linkout_json($genus,$species,$gene,$transcript) {
    $link =  [];

    if ($genus == 'Glycine' && $species == 'max') {
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
            'href' => 'http://phytozome.jgi.doe.gov/pz/portal.html#!results?search=0&crown=1&star=0&method=3253&searchText=' . $gene,
            'text' => 'view at Phytozome: ' . $gene,
        );
        $link[] = array(
            'href' => '/gbrowse_phavu1.0?query=name=' . $gene,
            'text' => 'view at LIS Pv1.0 GBrowse: ' . $gene,
        );
    }
    else if ($genus == 'Cicer' && $species == 'arietinum') {
        $link[] = array(
            'href' => '/gbrowse_cicar1.0?query=name=' . $gene,
            'text' => 'view at LIS Ca1.0 GBrowse: ' . $gene,
        );
    }
    else if ($genus == 'Cajanus' && $species == 'cajan') {
        $link[] = array(
            'href' => '/gbrowse_cajca1.0?query=name=' . $gene,
            'text' => 'view at LIS Cc1.0 GBrowse: ' . $gene,
        );
    }
    else if ($genus == 'Medicago' && $species == 'truncatula') {
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
            'href' => 'http://peanutbase.org/gbrowse_aradu1.0?query=name=' . $gene . ';dbid=gene_models',
            'text' => 'view at PeanutBase: ' . $gene,
        );            
    }
    else if ($genus == 'Arachis' && $species == 'ipaensis') {
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
        $link[] = array(
            'href' => 'http://maizegdb.org/cgi-bin/displaygenemodelrecord.cgi?id=' . $gene,
            'text' => 'view at MaizeGDB: ' . $gene,
        );
        $link[] = array(
            'href' => 'http://www.gramene.org/Zea_mays/Gene/Summary?g=' . $gene,
            'text' => 'view at Gramene: ' . $gene,
        );
    }

    drupal_json_output($link);  
    drupal_exit();
}


?>
