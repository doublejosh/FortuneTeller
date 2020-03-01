import React, { useMemo } from 'react'

export default props => {
	const findTotal = fortunes => {
		let votes = 0,
			shown = 0
		if (fortunes) {
			fortunes.forEach(f => {
				votes += f.votes
				shown += f.shown
			})
		}
		return { votes: votes, shown: shown }
	}

	const totals = useMemo(() => findTotal(props.fortunes), [props.fortunes])

	return (
		<section className="stats">
			{Math.round(
				(parseInt(totals.votes) / parseInt(totals.shown)) * 100
			)}
			% "Accurate" <br />({totals.votes}/{totals.shown})
		</section>
	)
}
