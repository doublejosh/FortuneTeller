import React from 'react'
import { css } from 'glamor'
import githubImg from '../static/img/github.svg'
import tumblrImg from '../static/img/tumblr.svg'

const links = [
	{
		img: githubImg,
		url: 'https://github.com/doublejosh/FortuneTeller',
		label: 'GitHub',
	},
	{
		img: tumblrImg,
		url: 'https://doublejosh.com',
		label: 'Blog',
	},
]

export default () => (
	<section
		{...css({
			'& ul': {
				listStyle: 'none',
				margin: '5rem 0 0 0',
				padding: 0,
				textAlign: 'center',
				'& li': {
					display: 'inline',
					marginRight: '2rem',
				},
			},
			'& img': {
				width: '40px',
			},
		})}>
		<ul>
			{links &&
				links.map((l, i) => (
					<li key={l.url}>
						<a href={l.url} target="_blank" rel="noopener noreferrer">
							<img src={l.img} alt={l.label} />
						</a>
					</li>
				))}
		</ul>
	</section>
)
